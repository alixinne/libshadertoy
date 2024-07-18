/* shadertoy screenhack for xscreensaver
 * Copyright (c) 2018 Alixinne <alixinne@pm.me>
 *
 * Based on the original hack dangerball by Jamie Zawinski <jwz@jwz.org>
 * for xscreensaver.
 */

#define DEFAULTS	"\n"

# define release_shadertoy 0
# define shadertoy_handle_event 0
#undef countof
#define countof(x) (sizeof((x))/sizeof((*x)))

#include "xlockmore.h"
#include <ctype.h>

#include "demo.h"

#define DEF_APIKEY      "fdnKWn"
#define DEF_SORT        "popular"
#define DEF_QUERY       "raymarching"

typedef struct {
	GLXContext *glx_context;
} shadertoy_configuration;

static shadertoy_configuration *bps = NULL;

static const char *opt_apikey;
static const char *opt_sort;
static const char *opt_query;

static XrmOptionDescRec opts[] = {
	{ "-apikey", ".apikey", XrmoptionSepArg, 0 },
	{ "-sort"  , ".sort"  , XrmoptionSepArg, 0 },
	{ "-query" , ".query" , XrmoptionSepArg, 0 }
};

static argtype vars[] = {
	{&opt_apikey, "apikey", "API Key"              , DEF_APIKEY, t_String},
	{&opt_sort  , "sort"  , "Search result sorting", DEF_SORT  , t_String},
	{&opt_query , "query" , "Search query"         , DEF_QUERY , t_String}
};

ENTRYPOINT ModeSpecOpt shadertoy_opts = {countof(opts), opts, countof(vars), vars, NULL};

/* Window management, etc
*/
	ENTRYPOINT void
reshape_shadertoy (ModeInfo *mi, int width, int height)
{
	shadertoy_resize(width, height);
}

	ENTRYPOINT void 
init_shadertoy (ModeInfo *mi)
{
	shadertoy_configuration *bp;

	MI_INIT (mi, bps);
	bp = &bps[MI_SCREEN(mi)];

	bp->glx_context = init_GL(mi);

	shadertoy_init(opt_apikey, opt_query, opt_sort, MI_WIDTH(mi), MI_HEIGHT(mi));
}

	ENTRYPOINT void
draw_shadertoy (ModeInfo *mi)
{
	shadertoy_configuration *bp = &bps[MI_SCREEN(mi)];
	Display *dpy = MI_DISPLAY(mi);
	Window window = MI_WINDOW(mi);

	if (!bp->glx_context)
		return;

	glXMakeCurrent(MI_DISPLAY(mi), MI_WINDOW(mi), *(bp->glx_context));

	shadertoy_render_frame();

	glXSwapBuffers(dpy, window);
}

	ENTRYPOINT void
free_shadertoy (ModeInfo *mi)
{
	shadertoy_free();
}

XSCREENSAVER_MODULE_2 ("Shadertoy", shadertoy, shadertoy)
