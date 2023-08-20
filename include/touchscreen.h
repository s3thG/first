#ifndef _TOUCHSCREEN_H
#define _TOUCHSCREEN_H

#define TOUCHSCREEN_GET_COORDINATE      0
#define TOUCHSCREEN_TOUCH_STATUS        1
#define TOUCHSCREEN_RELEASE_STATUS      2
#define TOUCHSCREEN_ERROR_STATUS        -1

extern int open_touchscreen_device(void);

extern int close_touchscreen_device(int ts_fd);

extern int get_touchscreen_coordinate(int ts_fd, int *get_x, int *get_y);

extern int ctrl_the_goods_page(int ts_fd, unsigned int *framebuffer_addr, shop_t *shop_head, goods_t *goods_head, bitmap *bm, font *f, unsigned int page_flag);

extern int ctrl_the_shopping_cart(int ts_fd, unsigned int *framebuffer_addr, shop_t *shop_head, goods_t *goods_head, bitmap *bm, font *f, unsigned int page_flag);

extern int ctrl_the_check_out(int ts_fd, unsigned int *framebuffer_addr, shop_t *shop_head, goods_t *goods_head, bitmap *bm, font *f, unsigned int page_flag);

extern int ctrl_the_details_page(int ts_fd, unsigned int *framebuffer_addr, shop_t *shop_head, goods_t *goods_head, bitmap *bm, font *f, unsigned int page_flag);

extern int ctrl_welcome_page(int ts_fd, unsigned int *framebuffer_addr);

extern int ctrl_admin_page(goods_t *goods_head, int ts_fd);

#endif/*end of _TOUCHSCREEN_H*/