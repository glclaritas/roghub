#ifndef FANCONTROL_H
#define FANCONTROL_H

/* set fanmode to next mode 0->1->2->1 */
int fanmode_toggle(void);

/* get current fanmode */
int fanmode_getid(void);

/* set fanmode to target mode */
int fanmode_setid(int targetmode);

/* display fanmode osd */
int show_fanmode(void);

extern char *TTP_FILE;

#endif
