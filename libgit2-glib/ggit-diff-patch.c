/*
 * ggit-diff-patch.c
 * This file is part of libgit2-glib
 *
 * Copyright (C) 2013 - Sindhu S
 *
 * libgit2-glib is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * libgit2-glib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libgit2-glib. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ggit-diff-patch.h"

struct _GgitDiffPatch
{
	git_diff_patch *diff_patch;
	gint ref_count;
};

G_DEFINE_BOXED_TYPE (GgitDiffPatch, ggit_diff_patch,
                     ggit_diff_patch_ref, ggit_diff_patch_unref)

GgitDiffPatch *
_ggit_diff_patch_wrap (git_diff_patch *diff_patch)
{
	GgitDiffPatch *gdiff_patch;

	gdiff_patch = g_slice_new (GgitDiffPatch);
	gdiff_patch->diff_patch = diff_patch;
	gdiff_patch->ref_count = 1;

	return gdiff_patch;
}

/**
 * ggit_diff_patch_ref:
 * @diff_patch: a #GgitDiffPatch.
 *
 * Atomically increments the reference count of @entry by one.
 * This function is MT-safe and may be called from any thread.
 *
 * Returns: (transfer none): a #GgitDiffPatch.
 */
GgitDiffPatch *
ggit_diff_patch_ref (GgitDiffPatch *diff_patch)
{
	g_return_val_if_fail (diff_patch != NULL, NULL);

	g_atomic_int_inc (&diff_patch->ref_count);

	return diff_patch;
}

/**
 * ggit_diff_patch_unref:
 * @diff_patch: a #GgitDiffPatch.
 *
 * Atomically decrements the reference count of @entry by one.
 * If the reference count drops to 0, @entry is freed.
 */
void
ggit_diff_patch_unref (GgitDiffPatch *diff_patch)
{
	g_return_if_fail (diff_patch != NULL);

	if (g_atomic_int_dec_and_test (&diff_patch->ref_count))
	{
		git_diff_patch_free (diff_patch->diff_patch);
		g_slice_free (GgitDiffPatch, diff_patch);
	}
}

/**
 * ggit_diff_patch_to_string:
 * @diff_patch: a #GgitDiffPatch.
 * @error: a #GError for error reporting, or %NULL.
 *
 * Gets the content of a patch as a single diff text.
 *
 * Returns: the content of a patch as a single diff text.
 */
gchar *
ggit_diff_patch_to_string (GgitDiffPatch  *diff_patch,
                           GError        **error)
{
	gchar *str;
	gint ret;
	gchar *gstr;

	g_return_val_if_fail (diff_patch != NULL, NULL);

	ret = git_diff_patch_to_str (&str, diff_patch->diff_patch);

	if (ret != GIT_OK)
	{
		return NULL;
	}

	gstr = g_strdup (str);
	free (str);

	return gstr;
}

/* ex:set ts=8 noet: */