#include <math.h>
#include "../include/illumination.h"
#include "../include/vector_math.h"

double clamp(double color_val){
    if (color_val < 0)
        return 0;
    else if (color_val > 1)
        return 1;
    else
        return color_val;
}
void scale_color(double* color, double scalar, double* out_color) {
    if (scalar < 0.0) {
        fprintf(stderr, "Error: scale_color: Can't apply negative scalar to a color value\n");
        exit(1);
    }
    out_color[0] = color[0] * scalar;
    out_color[1] = color[1] * scalar;
    out_color[2] = color[2] * scalar;
}

void copy_color(double* color, double* out_color) {
    out_color[0] = color[0];
    out_color[1] = color[1];
    out_color[2] = color[2];
}

void calculate_diffuse(double *N, double *L, double *IL, double *KD, double *out_color) {
   
    double n_dot_l = v3_dot(N, L);
    if (n_dot_l > 0) {
        double diffuse_product[3];
        diffuse_product[0] = KD[0] * IL[0];
        diffuse_product[1] = KD[1] * IL[1];
        diffuse_product[2] = KD[2] * IL[2];
        v3_scale(diffuse_product, n_dot_l, out_color);
    }
    else {
        out_color[0] = 0;
        out_color[1] = 0;
        out_color[2] = 0;
    }
}

void calculate_specular(double ns, double *L, double *R, double *N, double *V, double *KS, double *IL, double *out_color) {
    double v_dot_r = v3_dot(V, R);
    double n_dot_l = v3_dot(N, L);
    if (v_dot_r > 0 && n_dot_l > 0) {
        double vr_to_the_ns = pow(v_dot_r, ns);
        double spec_product[3];
        spec_product[0] = KS[0] * IL[0];
        spec_product[1] = KS[1] * IL[1];
        spec_product[2] = KS[2] * IL[2];
        v3_scale(spec_product, vr_to_the_ns, out_color);
    }
    else {
        v3_zero(out_color);
    }
}

double calculate_angular_att(Light *light, double direction_to_object[3]) {
    if (light->type != SPOTLIGHT)
        return 1.0;
    if (light->direction == NULL) {
        fprintf(stderr, "Error: calculate_angular_att: Can't have spotlight with no direction\n");
        exit(1);
    }
    normalize(light->direction);
    double theta_rad = light->theta_deg * (M_PI / 180.0);
    double cos_theta = cos(theta_rad);
    double vo_dot_vl = v3_dot(light->direction, direction_to_object);
    if (vo_dot_vl < cos_theta)
        return 0.0;
    return pow(vo_dot_vl, light->ang_att0);
}

double calculate_radial_att(Light *light, double distance_to_light) {
    if (light->rad_att0 == 0 && light->rad_att1 == 0 && light->rad_att2 == 0) {
        fprintf(stdout, "WARNING: calculate_radial_att: Found all 0s for attenuation. Assuming default values of radial attenuation\n");
        light->rad_att2 = 1.0;
    }
  
    if (distance_to_light > 99999999999999) return 1.0;

    double dl_sqr = sqr(distance_to_light);
    double denom = light->rad_att2 * dl_sqr + light->rad_att1 * distance_to_light + light->ang_att0;
    return 1.0 / denom;
}