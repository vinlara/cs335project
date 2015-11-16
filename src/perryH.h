#ifndef PERRYH_H
#define PERRYH_H

extern void loadTempFiles();
extern void cleanupTempFiles();
extern void initTextures(void);
extern void checkMouse(XEvent *e);
extern int checkKeys(XEvent *e);
extern void renderStartScreen();
extern void renderHelpScreen();
extern void renderGameOver();
extern void updateScore();
extern void render();
extern void normalize(Vec v);

#endif
