#pragma once

#include <array>
#include <functional>
#include <string>
#include <vector>
#include "definitions.h"

// �ڵ�ǰ��ʵ���У����������Ϊ�˻�����ڳ�ʼ�� AtmosphereParameters ����ȷ����
class Model {
public:
    Model(
        // �ṩ̫������ȡ�����ɢ�䡢����ɢ�䡢��������͵������������Ĳ���ֵ����λ�����ף���������˳������
        const std::vector<double> &wavelengths,
        // The solar irradiance at the top of the atmosphere, in W/m^2/nm. This
        // ����������̫�� irradiance���� vetor ������ wavelengths ��ͬ��С
        const std::vector<double> &solar_irradiance,
        // ̫���Ľǰ뾶����λ�ǻ���
        double sun_angular_radius,
        // ����뾶
        double bottom_radius,
        // ����뾶 + �����߶�
        double top_radius,
        // ���������ܶȷֲ�
        const std::vector<DensityProfileLayer> &rayleigh_density,
        // ��������ɢ��ϵ��
        const std::vector<double> &rayleigh_scattering,
        // ���ܽ��ܶȷֲ�
        const std::vector<DensityProfileLayer> &mie_density,
        // ���ܽ�ɢ��ϵ��
        const std::vector<double> &mie_scattering,
        // ���ܽ�����ϵ��
        const std::vector<double> &mie_extinction,
        // ���ܽ������ǶԳ���ϵ��
        double mie_phase_function_g,
        // �������ܶȷֲ�
        const std::vector<DensityProfileLayer> &absorption_density,
        // ��������ϵ��
        const std::vector<double> &absorption_extinction,
        // �����ƽ�� albedo���� vetor ������ wavelengths ��ͬ��С
        const std::vector<double> &ground_albedo,
        // ���ĵ�ƽ���춥��
        double max_sun_zenith_angle,
        // ģ��/����ϵ�ĵ�λ����һ�����������
        double length_unit_in_meters,
        // С�ڵ���3ʹ�� {kLambdaR, kLambdaG, kLambdaB} ������Ⱦ�������ֹ���
        unsigned int num_precomputed_wavelengths,
        // ��� single Mie.r �� Rayleigh and multiple scattering
        bool combine_scattering_textures,
        // �뾫��
        bool half_precision);

    static constexpr double kLambdaR = 680.0;
    static constexpr double kLambdaG = 550.0;
    static constexpr double kLambdaB = 440.0;

    // �����ɵ� GLSL �����ӡ�������ֶ���ʼ�� AtmosphereParameters
    void PrintAtmParameter();

private:
    typedef std::array<double, 3> vec3;
    typedef std::array<float, 9> mat3;

    unsigned int num_precomputed_wavelengths_;
    bool half_precision_;
    std::function<std::string(const vec3 &)> glsl_header_factory_;
    int transmittance_texture_;
    int scattering_texture_;
    int optional_single_mie_scattering_texture_;
    int irradiance_texture_;
    int atmosphere_shader_;
    int full_screen_quad_vao_;
    int full_screen_quad_vbo_;
};
