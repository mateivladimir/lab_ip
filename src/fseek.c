/* EWL
 * Copyright © 1995-2007 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2012/06/01 15:49:35 $
 * $Revision: 1.1 $
 */

/*
 *	Routines
 *	--------
 *		fseek
 *
 *		fsetpos
 */

#include <ansi_parms.h>

#include <errno.h>
#include <ansi_files.h>
#include <buffer_io.h>
#include <critical_regions.h>

#include <ewl_misra_types.h>

MISRA_EXCEPTION_RULE_16_2()

int_t _EWL_CDECL _fseek(FILE * _file, fpos_t offset, int_t mode)
{
	MISRA_EXCEPTION_RULE_11_4()
	struct _FILE * file = (struct _FILE *)_file;
	fpos_t		position;
	__pos_proc	pos_proc;


	if (!(file->mode.file_kind == (uint_t)__disk_file ) || file->state.error) {
		MISRA_EXCEPTION_RULE_20_5()
		errno = EFPOS;
		MISRA_EXCEPTION_RULE_14_7()
		return(-1);
	}

	if (file->state.io_state == (uint_t)__writing) {
		if (__flush_buffer(_file, NULL) != (int_t)__no_io_error) {
			set_error(file);
			MISRA_EXCEPTION_RULE_20_5()
			errno = EFPOS;
			MISRA_EXCEPTION_RULE_14_7()
			return(-1);
		}
	}

	if (mode == SEEK_CUR) {
		mode = SEEK_SET;
		if ((position = _ftell(_file)) == (fpos_t)-1) {
			position = 0U;
		}
		offset += position;
	}

	if ((mode != SEEK_END) && (file->mode.io_mode != (uint_t)__read_write_mode) &&
			((file->state.io_state == (uint_t)__reading) || (file->state.io_state == (uint_t)__rereading)))
	{	/* When in read mode, check for a position that is within the bounds of the current buffer */
		if ((offset >= file->position) || (offset < file->buffer_pos)) {
			/* not within buffer */
			file->state.io_state = (uchar_t)__neutral;
		} else {
			/* within the buffer, reset the buffer_ptr and buffer_len */
			file->buffer_ptr = file->buffer + (offset - file->buffer_pos);
			file->buffer_len = file->position - offset;
			file->state.io_state = (uchar_t)__reading;
		}
	} else {
		/* not reading */
		file->state.io_state = (uchar_t)__neutral;
	}

	if (file->state.io_state == (uchar_t)__neutral) {
		pos_proc = file->position_proc;
		if (pos_proc != 0) {
			if ((*pos_proc)(file->handle, &offset, mode))
			{
				set_error(file);
				MISRA_EXCEPTION_RULE_20_5()
				errno = EFPOS;
				MISRA_EXCEPTION_RULE_14_7()
				return(-1);
			}
		}

		file->state.eof      = 0U;
		file->position       = offset;
		file->buffer_len     = 0U;
	}

	return(0);
}

int_t _EWL_CDECL fseek(FILE * file, int32_t offset, int_t mode)
{
	fpos_t real_offset = (fpos_t)offset;
	int_t retval;
	MISRA_QUIET_UNUSED_SYMBOL()
	critical_regions crtrgn;

	if (file == stdin) {
		crtrgn = stdin_access;
	} else if (file == stdout) {
		crtrgn = stdout_access;
	} else if (file == stderr) {
		crtrgn = stderr_access;
	} else {
		crtrgn = files_access;
	}
  	__begin_critical_region(crtrgn);

	retval = _fseek(file, real_offset, mode);

  	__end_critical_region(crtrgn);

	return(retval);
}

int_t _EWL_CDECL fsetpos(FILE * file, const fpos_t * pos)
{
	int_t retval;
	MISRA_QUIET_UNUSED_SYMBOL()
	critical_regions crtrgn;

	if (file == stdin) {
		crtrgn = stdin_access;
	} else if (file == stdout) {
		crtrgn = stdout_access;
	} else if (file == stderr) {
		crtrgn = stderr_access;
	} else {
		crtrgn = files_access;
	}
  	__begin_critical_region(crtrgn);

	retval = _fseek(file, *pos, SEEK_SET);

  	__end_critical_region(crtrgn);

	return(retval);
}

