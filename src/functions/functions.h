#pragma once

#include "atmosphereParameters/constants.h"
#include "atmosphereParameters/definitions.h"
#include "util.h"

// ������������ľ���
Length DistanceToTopAtmosphereBoundary(IN(AtmosphereParameters) atmosphere, Length r, Number mu) {
    assert(r <= atmosphere.top_radius);
    assert(mu >= -1.0 && mu <= 1.0);
    const Area discriminant = r * r * (mu * mu - 1.0) + atmosphere.top_radius * atmosphere.top_radius;
    return ClampDistance(-r * mu + SafeSqrt(discriminant));
}

// ����ر�ľ���
Length DistanceToBottomAtmosphereBoundary(IN(AtmosphereParameters) atmosphere, Length r, Number mu) {
    assert(r >= atmosphere.bottom_radius);
    assert(mu >= -1.0 && mu <= 1.0);
    const Area discriminant = r * r * (mu * mu - 1.0) + atmosphere.bottom_radius * atmosphere.bottom_radius;
    return ClampDistance(-r * mu - SafeSqrt(discriminant));
}

// �����Ƿ�������ཻ
bool RayIntersectsGround(IN(AtmosphereParameters) atmosphere, Length r, Number mu) {
    assert(r >= atmosphere.bottom_radius);
    assert(mu >= -1.0 && mu <= 1.0);
    return (mu < 0.0) && (r * r * (mu * mu - 1.0) + atmosphere.bottom_radius * atmosphere.bottom_radius >= 0.0f * m2);
}

// ����ָ�������㼶�Ĵ����ܶ�
// �ڼ���ǳ�����������ʱ�ù�ʽ�˻�Ϊ Number = exp(layer.exp_scale * altitude);
// �ڼ������ʱ�ù�ʽ�˻�Ϊ Numer = layer.linear_term * altitude + layer.constant_term;
Number GetLayerDensity(IN(DensityProfileLayer) layer, Length altitude) {
    const Number density = layer.exp_term * exp(layer.exp_scale * altitude) + layer.linear_term * altitude + layer.constant_term;
    return clamp(density, Number(0.0), Number(1.0));
}

// �����������ݺ���ѡ��ͬ�Ĵ����㼶
Number GetProfileDensity(IN(DensityProfile) profile, Length altitude) {
    return altitude < profile.layers[0].width ?
        GetLayerDensity(profile.layers[0], altitude) :
        GetLayerDensity(profile.layers[1], altitude);
}

// ��������㵽���������������֮��Ĺ�ѧ����
Length ComputeOpticalLengthToTopAtmosphereBoundary(IN(AtmosphereParameters) atmosphere, IN(DensityProfile) profile, Length r, Number mu) {
    assert(r >= atmosphere.bottom_radius && r <= atmosphere.top_radius);
    assert(mu >= -1.0 && mu <= 1.0);
    // ��������
    const int SAMPLE_COUNT = 500;
    // ���ֵĳ���
    const Length dx = DistanceToTopAtmosphereBoundary(atmosphere, r, mu) / Number(SAMPLE_COUNT);
    Length result = 0.0 * m;
    for (int i = 0; i <= SAMPLE_COUNT; ++i) {
        const Length d_i = Number(i) * dx;
        // ��ǰ���������������ĵľ���
        const Length r_i = sqrt(d_i * d_i + 2.0 * r * mu * d_i + r * r);
        // ������Ĵ����ܶȣ����Դ����ײ��Ĵ����ܶȣ�
        const Number y_i = GetProfileDensity(profile, r_i - atmosphere.bottom_radius);
        // ����Ȩ��
        const Number weight_i = i == 0 || i == SAMPLE_COUNT ? 0.5 : 1.0;
        result += y_i * weight_i * dx;
    }
    return result;
}

// ��������㵽���������������֮���͸���ʣ���������ɢ�䡢����ɢ�䡢����
DimensionlessSpectrum ComputeTransmittanceToTopAtmosphereBoundary(IN(AtmosphereParameters) atmosphere, Length r, Number mu) {
    assert(r >= atmosphere.bottom_radius && r <= atmosphere.top_radius);
    assert(mu >= -1.0 && mu <= 1.0);
    // rayleigh_scattering == rayleigh_extinction������ɢ�䲻���չ�
    const DimensionlessSpectrum rayleighTerm = atmosphere.rayleigh_scattering * ComputeOpticalLengthToTopAtmosphereBoundary(atmosphere, atmosphere.rayleigh_density, r, mu);
    const DimensionlessSpectrum mieTerm = atmosphere.mie_extinction * ComputeOpticalLengthToTopAtmosphereBoundary(atmosphere, atmosphere.mie_density, r, mu);
    const DimensionlessSpectrum ozoneTerm = atmosphere.absorption_extinction * ComputeOpticalLengthToTopAtmosphereBoundary(atmosphere, atmosphere.absorption_density, r, mu);
    return exp(-(rayleighTerm + mieTerm + ozoneTerm));
}

// ��������ӳ��
Number GetUnitRangeFromTextureCoord(Number u, int texture_size) {
    return (u - 0.5 / Number(texture_size)) / (1.0 - 1.0 / Number(texture_size));
}

 // UV -> RMu
void GetRMuFromTransmittanceTextureUv(IN(AtmosphereParameters) atmosphere, IN(Vec2d) uv, OUT(Length) r, OUT(Number) mu) {
    assert(uv.x >= 0.0 && uv.x <= 1.0);
    assert(uv.y >= 0.0 && uv.y <= 1.0);
    Number x_mu = GetUnitRangeFromTextureCoord(uv.x, TRANSMITTANCE_TEXTURE_WIDTH);
    Number x_r = GetUnitRangeFromTextureCoord(uv.y, TRANSMITTANCE_TEXTURE_HEIGHT);
    // �����ƽ�ߵ����ߣ��ӵر����������ľ���
    Length H = sqrt(atmosphere.top_radius * atmosphere.top_radius - atmosphere.bottom_radius * atmosphere.bottom_radius);
    // �����ƽ�ߵ����ߣ�����㵽�ر�ľ���
    Length rho = H * x_r;
    r = sqrt(rho * rho + atmosphere.bottom_radius * atmosphere.bottom_radius);
    // ������㵽���������߽�ľ��룬���������� mu �ϵ���Сֵ��r, 1�������ֵ��r, mu_horizon��
    Length d_min = atmosphere.top_radius - r;
    Length d_max = rho + H;
    Length d = d_min + x_mu * (d_max - d_min);
    mu = d == 0.0 * m ? Number(1.0) : (H * H - rho * rho - d * d) / (2.0 * r * d);
    mu = ClampCosine(mu);
}

// TRANSMITTANCE
DimensionlessSpectrum ComputeTransmittanceToTopAtmosphereBoundaryTexture(IN(AtmosphereParameters) atmosphere, IN(Vec2d) frag_coord) {
    const Vec2d TRANSMITTANCE_TEXTURE_SIZE = Vec2d(TRANSMITTANCE_TEXTURE_WIDTH, TRANSMITTANCE_TEXTURE_HEIGHT);
    Length r;
    Number mu;
    GetRMuFromTransmittanceTextureUv(atmosphere, frag_coord / TRANSMITTANCE_TEXTURE_SIZE, r, mu);
    return ComputeTransmittanceToTopAtmosphereBoundary(atmosphere, r, mu);
}
