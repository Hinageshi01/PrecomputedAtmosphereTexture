#pragma once

#include "math/vec.h"

#define Length                    double
#define Wavelength                double
#define Angle                     double
#define SolidAngle                double
#define Power                     double
#define LuminousPower             double

#define Number                    double
#define InverseLength             double
#define Area                      double
#define Volume                    double
#define NumberDensity             double
#define Irradiance                double
#define Radiance                  double
#define SpectralPower             double
#define SpectralIrradiance        double
#define SpectralRadiance          double
#define SpectralRadianceDensity   double
#define ScatteringCoefficient     double
#define InverseSolidAngle         double
#define LuminousIntensity         double
#define Luminance                 double
#define Illuminance               double

// �������������͵�ͨ�ú���
#define AbstractSpectrum          Vec3d
// ���������ֵĺ���				  
#define DimensionlessSpectrum     Vec3d
// ���������� Power �ĺ���		  
#define PowerSpectrum             Vec3d
// ���������� Irradiance �ĺ���	  
#define IrradianceSpectrum        Vec3d
// ���������� Radiance �ĺ���	  
#define RadianceSpectrum          Vec3d
// ���������� Radiance �ܶȵĺ��� 
#define RadianceDensitySpectrum   Vec3d
// ������ɢ��ϵ��				   
#define ScatteringSpectrum        Vec3d

#define Position                  Vec3d
#define Direction                 Vec3d
#define Luminance3                Vec3d
#define Illuminance3              Vec3d

#define sampler2D int
#define sampler3D int
#define TransmittanceTexture      sampler2D
#define AbstractScatteringTexture sampler3D
#define ReducedScatteringTexture  sampler3D
#define ScatteringTexture         sampler3D
#define ScatteringDensityTexture  sampler3D
#define IrradianceTexture         sampler2D

// ����λ
constexpr Length m = 1.0;
constexpr Wavelength nm = 1.0;
constexpr Angle rad = 1.0;
constexpr SolidAngle sr = 1.0;
constexpr Power watt = 1.0;
constexpr LuminousPower lm = 1.0;

// ������������λ
constexpr double PI = 3.14159265358979323846;
constexpr Length km = 1000.0 * m;
constexpr Area m2 = m * m;
constexpr Volume m3 = m * m * m;
constexpr Angle pi = PI * rad;
constexpr Angle deg = pi / 180.0;
constexpr Irradiance watt_per_square_meter = watt / m2;
constexpr Radiance watt_per_square_meter_per_sr = watt / (m2 * sr);
constexpr SpectralIrradiance watt_per_square_meter_per_nm = watt / (m2 * nm);
constexpr SpectralRadiance watt_per_square_meter_per_sr_per_nm = watt / (m2 * sr * nm);
constexpr SpectralRadianceDensity watt_per_cubic_meter_per_sr_per_nm = watt / (m3 * sr * nm);
constexpr LuminousIntensity cd = lm / sr;
constexpr LuminousIntensity kcd = 1000.0 * cd;
constexpr Luminance cd_per_square_meter = cd / m2;
constexpr Luminance kcd_per_square_meter = kcd / m2;

// ���Ϊ width �Ĵ����㣬
// �ܶ��� 'exp_term' * exp('exp_scale' * h) + 'linear_term' * h + 'constant_term' ���壬
// clamp �� [0, 1]��h Ϊ���θ߶�
// ֻ�г������õ� linear_term �� constant_term
struct DensityProfileLayer {
	Length width;
	// �ǳ�����������Ϊ 1������Ϊ 0
	Number exp_term;
	// ����׼���θ߶ȷ�֮һ
	InverseLength exp_scale;
	InverseLength linear_term;
	Number constant_term;
};

// һ�����µ��������ɲ���Ӷ��ɵĴ����ܶȷֲ���
// ���һ��Ŀ�ȱ����ԣ�Ҳ����˵�������쵽���������ı߽磬
// �ܶȷֲ�ֵ�� [0, 1] ֮��仯
// ����˫�����ƽ��������ڳ����������Ĵ�����������ʲô���ζ�ֻʹ�� layers[1] ������
struct DensityProfile {
	DensityProfileLayer layers[2];
};

struct AtmosphereParameters {
	// ��������̫���� Irradiance
	IrradianceSpectrum solar_irradiance;
	// ̫���Ľǰ뾶
	// ע�⣺ʵ����ʹ�õĽ���ֵֻ�е��ýǶ�С�� 0.1 ʱ����Ч
	Angle sun_angular_radius;
	// ����뾶
	Length bottom_radius;
	// ����뾶 + �����߶�
	Length top_radius;

	// �������ӵ��ܶȷֲ������ɺ��θ߶ȵõ� [0, 1] ֮��ֵ�ĺ���
	DensityProfile rayleigh_density;
	// �������������ܶ����ĺ��θ߶ȣ�ͨ���Ǵ����ײ�����ɢ��ϵ�����ǲ����ĺ���
	// ���θ߶� h ����ɢ��ϵ��Ϊ 'rayleigh_scattering' ���� 'rayleigh_density'
	ScatteringSpectrum rayleigh_scattering;

	// ���ܽ����ܶȷֲ������ɺ��θ߶ȵõ� [0, 1] ֮��ֵ�ĺ���
	DensityProfile mie_density;
	// ���ܽ������ܶ����ĺ��θ߶ȣ�ͨ���Ǵ����ײ�����ɢ��ϵ�����ǲ����ĺ���
	// ���θ߶� h ����ɢ��ϵ��Ϊ 'mie_scattering' ���� 'mie_density'
	ScatteringSpectrum mie_scattering;
	// ���ܽ������ܶ����ĺ��θ߶ȣ�ͨ���Ǵ����ײ���������ϵ�����ǲ����ĺ���
	// ���θ߶� h ��������ϵ��Ϊ 'mie_extinction' ���� 'mie_density'
	ScatteringSpectrum mie_extinction;
	// Cornette-Shanks ���ܽ��ຯ���ķǶԳ��Բ���
	Number mie_phase_function_g;

	// �����չ��������ӣ�������������ܶȷֲ������ɺ��θ߶ȵõ� [0, 1] ֮��ֵ�ĺ���
	DensityProfile absorption_density;
	// ���չ�ķ��ӣ���������������ܶ����ĺ��θ߶�ʱ������ϵ����ʱ�����ĺ���
	// ���θ߶� h ��������ϵ��Ϊ 'absorption_extinction' ���� 'absorption_density'
	ScatteringSpectrum absorption_extinction;
	
	// �ر��ƽ�� albedo
	DimensionlessSpectrum ground_albedo;
	// ����Ԥ�������ɢ������̫���춥��
	// Ϊ�˻����󾫶ȣ�ʹ�ò����ɺ��Ե���� radiance ����С̫���춥�ǡ�
	// ���磬���ڵ���������102 ����һ���ܺõ�ѡ�� - ���� mu_s_min = -0.2
	Number mu_s_min;
};
