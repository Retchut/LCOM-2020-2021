PROG=proj

SRCS = proj.c vbe_funcs.c video.c menu.c highscore.c game.c logic.c sprite.c proj_timer.c rtc.c utils.c keyboard.c mouse.c

# __LCOM_OPTIMIZED__ disables most of the LCF's overhead (and functionality!)
CPPFLAGS += -pedantic #-D __LCOM_OPTIMIZED__

DPADD += ${LIBLCF}
LDADD += -llcf

.include <minix.lcom.mk>
