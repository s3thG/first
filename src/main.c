#include <stdio.h>
#include <stdlib.h>
#include <list.h>
#include <goods_info.h>
#include <shopping_cart.h>
#include <display.h>
#include <font.h>
#include <unistd.h>
#include <touchscreen.h>

struct resource
{
    goods_t *goods_head;
    shop_t *shop_head;
    font *f;   // 打开字体
    int ts_fd; //打开触摸屏
};

int main(void)
{
    //定义一个资源类型结构体
    struct resource src;
    /*
     *   初始化阶段
     */
    //商品信息链表头节点的申请
    src.goods_head = request_new_node();
    if (src.goods_head == NULL)
    {
        perror("头节点申请失败");
        goto request_head_node_err;
    }
    INIT_LIST_HEAD(&src.goods_head->list);
    //购物车信息链表头节点的申请
    src.shop_head = request_new_shop_cart_node();
    if (src.shop_head == NULL)
    {
        perror("购物车头节点申请失败");
        goto shop_cart_head_node_err;
    }
    INIT_LIST_HEAD(&src.shop_head->s_list);

    start_input_goods_list(src.goods_head);
    //用户界面阶段
    int x, y;
    int status;
    int framebuffer_fd;
    unsigned int *framebuffer_addr;
    int cc_cnt_x = 400, cc_cnt_y = 240;
    int x_speed = 2, y_speed = 2;

    //打开显存设备
    framebuffer_fd = open_framebuffer_device(&framebuffer_addr);
    //打开字库文件
    src.f = fontLoad("/usr/share/fonts/DroidSansFallback.ttf");
    //打开触摸屏
    src.ts_fd = open_touchscreen_device();
    //字体大小的设置
    fontSetSize(src.f, 50);
    //点阵图初始化
    bitmap bm;
    bm.height = 480;
    bm.width = 800;
    bm.byteperpixel = 4;
    bm.map = (void *)framebuffer_addr;
    //显示销售界面
    // display_vending_window(framebuffer_addr, &bm, src.f, src.goods_head, 1);
    while (1)
    {
        ctrl_welcome_page(src.ts_fd,framebuffer_addr);
        ctrl_the_goods_page(src.ts_fd, framebuffer_addr, src.shop_head, src.goods_head, &bm, src.f, 1);
    }

    close_touchscreen_device(src.ts_fd);
    close_framebuffer_device(framebuffer_fd, framebuffer_addr);
    //关闭字体，关闭画板
    fontUnload(src.f);
    return 0;

request_head_node_err:
shop_cart_head_node_err:
    return -1;
}