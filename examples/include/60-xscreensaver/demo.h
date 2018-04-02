#ifndef _DEMO_H_
#define _DEMO_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int shadertoy_init(const char *api_key, const char *query, const char *sort, int width, int height);

void shadertoy_resize(int width, int height);

void shadertoy_render_frame();

void shadertoy_free();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _DEMO_H_ */
