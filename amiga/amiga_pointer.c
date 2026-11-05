/* The mouse position on a full screen, read from Intuition.
 *
 * Full screen, Commander Genius draws into a renderer with a logical size
 * (SDL_RenderSetLogicalSize) that SDL scales up to the screen, and it
 * expects SDL to give the mouse events in those logical units. With
 * OpenGPU's SDL2.module on a big RTG screen (1920x1200 in the lab) the
 * clickable places were away from the pointer: the module's events and
 * the picture don't use the same scale. So here the pointer's place on
 * the window, as Intuition has it, is turned into logical units the way
 * SDL places the picture: the renderer's output stretched over the
 * Intuition window, the logical size scaled into it (whole steps when
 * integer scaling is on) and centred.
 * The AmiCradle Team. GPL 2 or later, like Commander Genius. */
#undef REGISTER
#include <exec/types.h>
#include <intuition/intuitionbase.h>
#include <intuition/screens.h>
#include <proto/exec.h>
#include <proto/intuition.h>

extern struct IntuitionBase *IntuitionBase;

/* out_w/out_h: the renderer's output size; log_w/log_h: its logical size.
 * Gives 1 and *x, *y in logical units (and the Intuition window's inner
 * size in *win_w, *win_h, for the log), or 0 (then the caller keeps SDL's
 * numbers) when there is no active window off the Workbench screen. */
int amiga_pointer_logical(int out_w, int out_h, int log_w, int log_h,
                          int integer_scale, int *x, int *y,
                          int *win_w, int *win_h)
{
	struct Window *w;
	ULONG lock;
	int mx, my, iw, ih;
	float px, py, scale, vw, vh, vx, vy;

	if (out_w <= 0 || out_h <= 0 || log_w <= 0 || log_h <= 0)
		return 0;

	lock = LockIBase(0);
	w = IntuitionBase->ActiveWindow;
	if (!w || !w->WScreen || (w->WScreen->Flags & SCREENTYPE) == WBENCHSCREEN) {
		UnlockIBase(lock);
		return 0;
	}
	mx = w->MouseX - w->BorderLeft;
	my = w->MouseY - w->BorderTop;
	iw = w->Width - w->BorderLeft - w->BorderRight;
	ih = w->Height - w->BorderTop - w->BorderBottom;
	UnlockIBase(lock);

	if (iw <= 0 || ih <= 0)
		return 0;

	/* Intuition's window -> the renderer's output (the same when the
	 * module draws 1:1). */
	px = (float)mx * (float)out_w / (float)iw;
	py = (float)my * (float)out_h / (float)ih;

	/* SDL's logical-size viewport in the output. */
	{
		float sx = (float)out_w / (float)log_w;
		float sy = (float)out_h / (float)log_h;
		scale = sx < sy ? sx : sy;
		if (integer_scale && scale >= 1.0f)
			scale = (float)(int)scale;
	}
	vw = (float)log_w * scale;
	vh = (float)log_h * scale;
	vx = ((float)out_w - vw) / 2.0f;
	vy = ((float)out_h - vh) / 2.0f;

	*x = (int)((px - vx) / scale);
	*y = (int)((py - vy) / scale);

	if (win_w) *win_w = iw;
	if (win_h) *win_h = ih;
	return 1;
}
