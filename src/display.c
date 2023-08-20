#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>
#include <display.h>
#include <font.h>
#include <goods_info.h>
#include <shopping_cart.h>

/*
    函数功能：
        打开显存文件及映射显存地址
    参数：
        framebuffer_addr：用来存放显存的地址
    返回值：
        返回一个显存的文件描述符，失败返回-1
*/
int open_framebuffer_device(unsigned int **framebuffer_addr)
{
    int framebuffer_fd;

    //打开显存文件
    framebuffer_fd = open("/dev/fb0", O_RDWR);
    if (framebuffer_fd == -1)
    {
        fprintf(stderr, "打开显存设备文件失败:%s", strerror(errno));
        return -1;
    }

    //映射显存地址
    *framebuffer_addr = mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, framebuffer_fd, 0);
    if (*framebuffer_addr == MAP_FAILED)
    {
        fprintf(stderr, "映射显存设备文件失败:%s", strerror(errno));
        goto map_framebuffer_err;
    }

    return framebuffer_fd;

map_framebuffer_err:
    close(framebuffer_fd);

    return -1;
}

int close_framebuffer_device(int framebuffer_fd, unsigned int *framebuffer_addr)
{
    //取消映射显存地址
    munmap(framebuffer_addr, 800 * 480 * 4);

    //关闭显存文件
    return close(framebuffer_fd);
}

//设置界面背景
int display_backgroud(unsigned int *frambuffer_addr)
{
    display_jpeg_format_to_framebuffer("background.jpg", 0, 0, frambuffer_addr, 1);
}

//设置商品底色
int set_bottom_color(int pos_x, int pos_y, unsigned int *frambuffer_addr)
{
    display_jpeg_format_to_framebuffer("frame.jpg", pos_x, pos_y, frambuffer_addr, 1);
}

//显示商品图片,文字信息
int display_goods_picture(const char *path, int pos_x, int pos_y, int *frambuffer_addr)
{
    display_jpeg_format_to_framebuffer(path, pos_x, pos_y, frambuffer_addr, 1);
}

//销售界面显示
int display_vending_window(unsigned int *frambuffer_addr, bitmap *bm, font *f, goods_t *goods_head, unsigned int page_flag)
{
    goods_t *cur_node;
    int total_cnt = total_number_of_goods(goods_head); //得到商品总数
    int total_page = total_cnt / 6;
    if (total_cnt % 6 != 0)
    {
        total_page++;
    }
    if (total_page > page_flag)
    {
        //显示没有更多页了
        printf("当前已经是最后一页了\n");
    }
    if (page_flag == 1)
    {
        display_jpeg_format_to_framebuffer("背景1.jpg", 0, 0, frambuffer_addr, 1);
    }
    else if (page_flag < total_page && page_flag > 1)
    {
        display_jpeg_format_to_framebuffer("背景中.jpg", 0, 0, frambuffer_addr, 1);
    }
    else if (page_flag == total_page && page_flag > 1)
    {
        display_jpeg_format_to_framebuffer("背景尾.jpg", 0, 0, frambuffer_addr, 1);
    }

    printf("当前商品总数：%d当前商品页数%d\n", total_cnt, total_page);
    unsigned int id;
    char buffer[100];
    fontSetSize(f, 25);
    // display_backgroud(frambuffer_addr);
    int row, col, up_limit, cycle_cnt, i; //加入循环计数器，防止死循环
    id = 1, up_limit = 20, cycle_cnt = 0;
    //空循环到目标页
    /*
    for (i = 0; i < page_flag - 1; i++)
    {
        for (row = 0; row < 2; row++)
        {
            for (col = 0; col < 3; col++, id++)
            {
                cur_node = serach_specific_goods(goods_head, id);
                if (cur_node == NULL)
                {
                    col--;
                    cycle_cnt++;
                    if (cycle_cnt >= up_limit)
                    {
                        break;
                    }
                    continue; //有死循环风险，应该加入一个上限中止死循环
                }
            }
            cycle_cnt = 0;
        }
    }
    for (row = 0; row < 2; row++)
    {
        for (col = 0; col < 3; col++, id++)
        {
            cur_node = serach_specific_goods(goods_head, id);
            if (cur_node == NULL)
            {
                col--;
                cycle_cnt++;
                if (cycle_cnt >= up_limit)
                {
                    break;
                }
                continue; //有死循环风险，应该加入一个上限中止死循环
            }
            else
            {
                set_bottom_color(40 + col * 180, 60 + row * 210, frambuffer_addr);
                sprintf(buffer, "%s.jpg", cur_node->name);
                display_goods_picture(buffer, 48 + col * 180, 68 + row * 210, frambuffer_addr);
                fontPrint(f, bm, 80 + col * 180, 183 + row * 210, cur_node->name, getColor(0, 0, 0, 255), 0);
                fontPrint(f, bm, 45 + col * 180, 210 + row * 210, "售价：", getColor(0, 0, 0, 255), 0);
                sprintf(buffer, "%.2f", cur_node->price);
                fontPrint(f, bm, 100 + col * 180, 210 + row * 210, buffer, getColor(0, 0, 0, 255), 0);
                display_goods_picture("购物车.jpg", 160 + col * 180, 210 + row * 210, frambuffer_addr);
                fontPrint(f, bm, 45 + col * 180, 240 + row * 210, "库存：", getColor(0, 0, 0, 255), 0);
                sprintf(buffer, "%d", cur_node->stock);
                fontPrint(f, bm, 100 + col * 180, 240 + row * 210, buffer, getColor(0, 0, 0, 255), 0);
            }
        }
    }
    */

    goods_t *pos;
    int cnt = 0;
    i = 0;
    col = 0;
    row = 0;
    list_for_each_entry(cur_node, &goods_head->list, list)
    {
        printf("%s\n", cur_node->name);
        if (cnt < (page_flag - 1) * 6) //跳过前面的页数
        {
            cnt++;
            continue;
        }
        else
        {
            printf("%d\n", col);
            set_bottom_color(40 + col * 180, 60 + row * 210, frambuffer_addr);
            sprintf(buffer, "%s.jpg", cur_node->name);
            display_goods_picture(buffer, 48 + col * 180, 68 + row * 210, frambuffer_addr);
            fontPrint(f, bm, 80 + col * 180, 183 + row * 210, cur_node->name, getColor(0, 0, 0, 255), 0);
            fontPrint(f, bm, 45 + col * 180, 210 + row * 210, "售价：", getColor(0, 0, 0, 255), 0);
            sprintf(buffer, "%.2f", cur_node->price);
            fontPrint(f, bm, 100 + col * 180, 210 + row * 210, buffer, getColor(0, 0, 0, 255), 0);
            display_goods_picture("购物车.jpg", 160 + col * 180, 210 + row * 210, frambuffer_addr);
            fontPrint(f, bm, 45 + col * 180, 240 + row * 210, "库存：", getColor(0, 0, 0, 255), 0);
            sprintf(buffer, "%d", cur_node->stock);
            fontPrint(f, bm, 100 + col * 180, 240 + row * 210, buffer, getColor(0, 0, 0, 255), 0);
            col++;
            i++;
            if (col == 3)
            {
                row++;
                col = 0;
            }
            if (i == 6 || row == 2)
            {
                break;
            }
        }
    }
    return 0;
}

//购物车界面
int display_shopping_cart_page(shop_t *shop_head, goods_t *goods_head, bitmap *bm, font *f, unsigned int page_flag, unsigned int *framebuffer_addr)
{
    int total_kinds = foreach_shopping_cart(shop_head);
    int total_page = total_kinds / 4;
    if (total_kinds % 4 != 0)
    {
        total_page++;
    }
    //分别显示不同情况下的购物车背景
    if (total_page == 1)
    {
        display_jpeg_format_to_framebuffer("购物车onlyone.jpg", 50, 20, framebuffer_addr, 1);
    }
    else if (total_page > 1 && page_flag == 1)
    {
        display_jpeg_format_to_framebuffer("购物车firstpage.jpg", 50, 20, framebuffer_addr, 1);
    }
    else if (total_page == page_flag)
    {
        display_jpeg_format_to_framebuffer("购物车有多页的last.jpg", 50, 20, framebuffer_addr, 1);
    }
    else if (total_page == 0)
    {
        printf("购物车为空\n");
        display_jpeg_format_to_framebuffer("空购物车背景.jpg", 50, 20, framebuffer_addr, 1);
    }
    else
    {
        display_jpeg_format_to_framebuffer("购物车背景.jpg", 50, 20, framebuffer_addr, 1);
    }

    char *buffer = (char *)malloc(100);
    int cnt = 0;
    int cur_goods_on_page; //当前购物车已经打印
    shop_t *pos;
    goods_t *temp;
    fontSetSize(f, 35);
    int i = 0; //
    float sum = 0;
    // display_jpeg_format_to_framebuffer("购物车背景.jpg", 50, 20, framebuffer_addr, 1);
    list_for_each_entry(pos, &shop_head->s_list, s_list)
    {

        if (cnt < (page_flag - 1) * 4) //跳过前面的页数
        {
            cnt++;
            continue;
        }
        else
        {
            strcpy(buffer, pos->name);
            fontSetSize(f, 30);
            fontPrint(f, bm, 120, 160 + i * 42, buffer, getColor(0, 0, 0, 0), 0);
            fontSetSize(f, 35);
            sprintf(buffer, "单价：%.2f 元", pos->price);
            fontPrint(f, bm, 250, 155 + i * 40, buffer, getColor(0, 0, 255, 0), 0);
            display_jpeg_format_to_framebuffer("add.jpg", 430, 165 + i * 40, framebuffer_addr, 1);
            sprintf(buffer, "X%d", pos->purchase_cnt);
            fontPrint(f, bm, 460, 155 + i * 40, buffer, getColor(0, 0, 0, 255), 0);
            display_jpeg_format_to_framebuffer("minus.jpg", 510, 165 + i * 40, framebuffer_addr, 1);
            sprintf(buffer, "小计：%.2f", pos->price * pos->purchase_cnt);
            fontPrint(f, bm, 550, 155 + i * 40, buffer, getColor(0, 0, 0, 255), 0);
            i++;
            if (i >= 4)
            {
                break;
            }
        }
    }
    sum = whole_cart_cost_sum(shop_head);
    sprintf(buffer, "%.2f", sum);
    fontSetSize(f, 40);
    fontPrint(f, bm, 590, 332, buffer, getColor(0, 0, 0, 255), 0);
    free(buffer);
    return 0;
}

int display_check_out_page(shop_t *shop_head, goods_t *goods_head, bitmap *bm, font *f, unsigned int page_flag, unsigned int *framebuffer_addr)
{
    int total_kinds = foreach_shopping_cart(shop_head);
    int total_page = total_kinds / 4;
    if (total_kinds % 4 != 0)
    {
        total_page++;
    }
    //分别显示不同情况下的购物车背景
    if (total_page == 1)
    {
        display_jpeg_format_to_framebuffer("付款页onlyone.jpg", 50, 20, framebuffer_addr, 1);
    }
    else if (total_page > 1 && page_flag == 1)
    {
        display_jpeg_format_to_framebuffer("付款页firstpage.jpg", 50, 20, framebuffer_addr, 1);
    }
    else if (total_page == page_flag)
    {
        display_jpeg_format_to_framebuffer("付款页有多页的last.jpg", 50, 20, framebuffer_addr, 1);
    }
    else if (total_page == 0)
    {
        return 0;
    }
    else
    {
        display_jpeg_format_to_framebuffer("付款页背景.jpg", 50, 20, framebuffer_addr, 1);
    }

    char *buffer = (char *)malloc(100);
    int cnt = 0;
    int cur_goods_on_page; //当前购物车已经打印
    shop_t *pos;
    goods_t *temp;
    fontSetSize(f, 35);
    int i = 0; //
    float sum = 0;
    // display_jpeg_format_to_framebuffer("购物车背景.jpg", 50, 20, framebuffer_addr, 1);
    list_for_each_entry(pos, &shop_head->s_list, s_list)
    {

        if (cnt < (page_flag - 1) * 4) //跳过前面的页数
        {
            cnt++;
            continue;
        }
        else
        {
            strcpy(buffer, pos->name);
            fontSetSize(f, 30);
            fontPrint(f, bm, 120, 160 + i * 42, buffer, getColor(0, 0, 0, 0), 0);
            fontSetSize(f, 35);
            sprintf(buffer, "单价：%.2f 元", pos->price);
            fontPrint(f, bm, 250, 155 + i * 40, buffer, getColor(0, 0, 255, 0), 0);
            sprintf(buffer, "X%d", pos->purchase_cnt);
            fontPrint(f, bm, 460, 155 + i * 40, buffer, getColor(0, 0, 0, 255), 0);
            sprintf(buffer, "小计：%.2f", pos->price * pos->purchase_cnt);
            fontPrint(f, bm, 550, 155 + i * 40, buffer, getColor(0, 0, 0, 255), 0);
            i++;
            if (i >= 4)
            {
                break;
            }
        }
    }
    sum = whole_cart_cost_sum(shop_head);
    sprintf(buffer, "%.2f", sum);
    fontSetSize(f, 40);
    fontPrint(f, bm, 590, 332, buffer, getColor(0, 0, 0, 255), 0);
    free(buffer);
    return 0;
}

int display_goods_detail_page(shop_t *shop_head, goods_t *goods_head, bitmap *bm, font *f, unsigned int page_flag, unsigned int *framebuffer_addr)
{
    int total_kinds = total_number_of_goods(goods_head);
    int total_page = total_kinds;
    char *buffer = (char *)malloc(100);
    int cnt = 0;
    int cur_goods_on_page; //当前购物车已经打印
    shop_t *pos;
    goods_t *temp;
    fontSetSize(f, 35);

    if (page_flag == 1 && total_page == 1)
    {
        display_jpeg_format_to_framebuffer("详情only1.jpg", 0, 0, framebuffer_addr, 1);
    }
    else if (page_flag == total_kinds && total_kinds > 1)
    {
        display_jpeg_format_to_framebuffer("详情最后.jpg", 0, 0, framebuffer_addr, 1);
    }
    else if (page_flag > 1 && page_flag < total_page)
    {
        display_jpeg_format_to_framebuffer("详情多页.jpg", 0, 0, framebuffer_addr, 1);
    }
    else if (page_flag == 1 && total_page > 1)
    {
        display_jpeg_format_to_framebuffer("详情第一.jpg", 0, 0, framebuffer_addr, 1);
    }
    temp = serach_specific_goods_in_orders(goods_head, page_flag);
    sprintf(buffer, "%s.jpg", temp->name);
    display_jpeg_format_to_framebuffer(buffer, 320, 150, framebuffer_addr, 1.4);
    fontSetSize(f, 50);
    fontPrint(f, bm, 320, 100, temp->name, getColor(0, 0, 0, 255), 0);
    fontSetSize(f, 40);
    fontPrint(f, bm, 410, 340, "售价：", getColor(0, 0, 0, 255), 0);
    sprintf(buffer, "%.2f", temp->price);
    fontPrint(f, bm, 510, 340, buffer, getColor(0, 0, 0, 255), 0);
    fontPrint(f, bm, 190, 340, "库存：", getColor(0, 0, 0, 255), 0);
    sprintf(buffer, "%d", temp->stock);
    fontPrint(f, bm, 290, 340, buffer, getColor(0, 0, 0, 255), 0);
    if (temp->stock > 0)
    {
        display_jpeg_format_to_framebuffer("详情车.jpg", 420, 390, framebuffer_addr, 1);
    }
    else if (temp->stock == 0)
    {
        fontPrint(f, bm, 380, 390, "库存不足！", getColor(0, 0, 0, 255), 0);
    }
    return 0;
}

