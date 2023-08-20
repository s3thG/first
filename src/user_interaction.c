#include <stdio.h>
#include <stdlib.h>
#include <goods_info.h>
#include <shopping_cart.h>
#include <list.h>
#include <display.h>
#include <touchscreen.h>
#include <unistd.h>


//大便增加宏函数，能不动别动
#define ADD_GOODS_COUNT_IN_CART(target, page_flag, shop_head, goods_head, framebuffer_addr)        \
    do                                                                                             \
    {                                                                                              \
        temp = get_pos_at_shopping_list(target + 4 * (page_flag - 1), shop_head);                  \
        if (temp != NULL)                                                                          \
        {                                                                                          \
            int ret = stock_info_preview(goods_head, temp->goods_id, temp->purchase_cnt);          \
            if (ret == 0)                                                                          \
            {                                                                                      \
                change_goods_count_in_cart(1, temp->goods_id, shop_head, goods_head);              \
                goto back_from_former_page;                                                        \
            }                                                                                      \
            else if (ret == -1)                                                                    \
            {                                                                                      \
                display_jpeg_format_to_framebuffer("库存不足.jpg", 280, 370, framebuffer_addr, 1); \
                usleep(500000);                                                                    \
                goto back_from_former_page;                                                        \
            }                                                                                      \
        }                                                                                          \
    } while (0)

#define DEC_GOODS_COUNT_IN_CART(line, goods_head, shop_head, page_flag, framebuffer_addr)          \
    do                                                                                             \
    {                                                                                              \
        temp = get_pos_at_shopping_list(line + 4 * (page_flag - 1), shop_head);                    \
        if (temp != NULL && temp->purchase_cnt != 1)                                               \
        {                                                                                          \
            int ret = stock_info_preview(goods_head, temp->goods_id, temp->purchase_cnt);          \
            if (ret == 0)                                                                          \
            {                                                                                      \
                change_goods_count_in_cart(-1, temp->goods_id, shop_head, goods_head);             \
                goto back_from_former_page;                                                        \
            }                                                                                      \
            else if (ret == -1)                                                                    \
            {                                                                                      \
                display_jpeg_format_to_framebuffer("库存不足.jpg", 280, 370, framebuffer_addr, 1); \
                usleep(500000);                                                                    \
                goto back_from_former_page;                                                        \
            }                                                                                      \
        }                                                                                          \
        else if (temp != NULL && temp->purchase_cnt == 1)                                          \
        {                                                                                          \
            del_cart_specific_goods(temp->goods_id, shop_head);                                    \
            temp = get_pos_at_shopping_list(1 + 4 * (page_flag - 1), shop_head);                   \
            if (temp == NULL && page_flag != 1)                                                    \
            {                                                                                      \
                return 1;                                                                          \
            }                                                                                      \
            else                                                                                   \
            {                                                                                      \
                goto back_from_former_page;                                                        \
            }                                                                                      \
        }                                                                                          \
    } while (0)


//控制购买界面
int ctrl_the_goods_page(int ts_fd, unsigned int *framebuffer_addr, shop_t *shop_head, goods_t *goods_head, bitmap *bm, font *f, unsigned int page_flag)
{
    int x, y;      //  触摸屏坐标
    int flag = -1; //排序方式变换
    int status;
    int ret;
    int total_cnt;
    int total_page;
    int secret_num;
jump_page:
    total_cnt = total_number_of_goods(goods_head); //得到商品总数
    total_page = total_cnt / 6;
    if (total_cnt % 6 != 0)
    {
        total_page++;
    }
    if (total_page < page_flag)
    {
        //显示没有更多页了
        printf("当前已经是最后一页了\n");
        page_flag = 1;
        goto jump_page;
    }
back_from_cart: //从购物车回来
    secret_num = 0;
    printf("当前page_flag:%d\n", page_flag);
    display_vending_window(framebuffer_addr, bm, f, goods_head, page_flag);
    display_jpeg_format_to_framebuffer("黄向上箭头.jpg", 0, 0, framebuffer_addr, 1);
    display_jpeg_format_to_framebuffer("黄向下箭头.jpg", 30, 0, framebuffer_addr, 1);
    while (1)
    {
        status = get_touchscreen_coordinate(ts_fd, &x, &y);
        if (status == TOUCHSCREEN_TOUCH_STATUS)
        {
            if ((x > 655 && x < 790) && (y > 210 && y < 310) && (page_flag >= 1)) //下一页
            {
                // ctrl_the_goods_page(ts_fd, framebuffer_addr, shop_head, goods_head, bm, f, page_flag + 1);
                page_flag++;
                goto jump_page;
            }
            else if ((x > 630 && x < 790) && (y > 90 && y < 190) && (page_flag > 1)) //上一页
            {
                // ctrl_the_goods_page(ts_fd, framebuffer_addr, shop_head, goods_head, bm, f, page_flag - 1);
                page_flag--;
                goto jump_page;
            }
            else if ((x > 160 && x < 190) && (y > 200 && y < 240))
            {
                if ((1 + (page_flag - 1) * 6) > total_cnt)
                {
                    continue;
                }
                ret = add_goods_in_shopping_cart(1, 1 + (page_flag - 1) * 6, shop_head, goods_head);
                if (ret == -1)
                {
                    display_jpeg_format_to_framebuffer("库存不足.jpg", 190, 240, framebuffer_addr, 1);
                    sleep(1);
                    goto back_from_cart;
                }
                display_jpeg_format_to_framebuffer("加一.jpg", 190, 240, framebuffer_addr, 1);
                usleep(250000);
                goto back_from_cart;
            }
            else if ((x > 330 && x < 370) && (y > 200 && y < 240)) //(1,2)
            {
                if ((2 + (page_flag - 1) * 6) > total_cnt)
                {
                    continue;
                }
                ret = add_goods_in_shopping_cart(1, 2 + (page_flag - 1) * 6, shop_head, goods_head);
                if (ret == -1)
                {
                    display_jpeg_format_to_framebuffer("库存不足.jpg", 370, 240, framebuffer_addr, 1);
                    sleep(1);
                    goto back_from_cart;
                }
                display_jpeg_format_to_framebuffer("加一.jpg", 370, 240, framebuffer_addr, 1);
                usleep(250000);
                goto back_from_cart;
            }
            else if ((x > 520 && x < 550) && (y > 200 && y < 240)) //(1,3)
            {
                if ((3 + (page_flag - 1) * 6) > total_cnt)
                {
                    continue;
                }
                ret = add_goods_in_shopping_cart(1, 3 + (page_flag - 1) * 6, shop_head, goods_head);
                if (ret == -1)
                {
                    display_jpeg_format_to_framebuffer("库存不足.jpg", 550, 240, framebuffer_addr, 1);
                    sleep(1);
                    goto back_from_cart;
                }
                display_jpeg_format_to_framebuffer("加一.jpg", 550, 240, framebuffer_addr, 1);
                usleep(250000);
                goto back_from_cart;
            }
            else if ((x > 150 && x < 190) && (y > 420 && y < 450)) //(2,1)
            {
                if ((4 + (page_flag - 1) * 6) > total_cnt)
                {
                    continue;
                }
                ret = add_goods_in_shopping_cart(1, 4 + (page_flag - 1) * 6, shop_head, goods_head);
                if (ret == -1)
                {
                    display_jpeg_format_to_framebuffer("库存不足.jpg", 190, 430, framebuffer_addr, 1);
                    sleep(1);
                    goto back_from_cart;
                }
                display_jpeg_format_to_framebuffer("加一.jpg", 190, 420, framebuffer_addr, 1);
                usleep(250000);
                goto back_from_cart;
            }
            else if ((x > 330 && x < 370) && (y > 420 && y < 450)) //(2,2)
            {
                if ((5 + (page_flag - 1) * 6) > total_cnt)
                {
                    continue;
                }
                ret = add_goods_in_shopping_cart(1, 5 + (page_flag - 1) * 6, shop_head, goods_head);
                if (ret == -1)
                {
                    display_jpeg_format_to_framebuffer("库存不足.jpg", 370, 430, framebuffer_addr, 1);
                    sleep(1);
                    goto back_from_cart;
                }
                display_jpeg_format_to_framebuffer("加一.jpg", 370, 420, framebuffer_addr, 1);
                usleep(250000);
                goto back_from_cart;
            }
            else if ((x > 510 && x < 550) && (y > 420 && y < 450)) //(3,3)
            {
                if ((6 + (page_flag - 1) * 6) > total_cnt)
                {
                    continue;
                }
                ret = add_goods_in_shopping_cart(1, 6 + (page_flag - 1) * 6, shop_head, goods_head);
                if (ret == -1)
                {
                    display_jpeg_format_to_framebuffer("库存不足.jpg", 550, 430, framebuffer_addr, 1);
                    sleep(1);
                    goto back_from_cart;
                }
                display_jpeg_format_to_framebuffer("加一.jpg", 550, 420, framebuffer_addr, 1);
                usleep(250000);
                goto back_from_cart;
            }
            else if ((x > 670 && x < 800) && (y > 340 && y < 480)) //购物车
            {
                int ret = ctrl_the_shopping_cart(ts_fd, framebuffer_addr, shop_head, goods_head, bm, f, 1);
                if (ret == 7)
                {
                    output_goods_info_to_file(goods_head);
                    // ctrl_the_goods_page(ts_fd, framebuffer_addr, shop_head, goods_head, bm, f, 1);
                    // page_flag = 1;
                    // goto back_from_cart;
                    return 0;
                }
                else
                {
                    goto back_from_cart;
                }
            }
            else if ((x > 45 && x < 190) && (y > 68 && y < 190))
            {
                if ((1 + (page_flag - 1) * 6) > total_cnt)
                {
                    continue;
                }
                ctrl_the_details_page(ts_fd, framebuffer_addr, shop_head, goods_head, bm, f, (1 + (page_flag - 1) * 6));
                goto back_from_cart;
            }
            else if ((x > 225 && x < 365) && (y > 68 && y < 190))
            {
                if ((2 + (page_flag - 1) * 6) > total_cnt)
                {
                    continue;
                }
                ctrl_the_details_page(ts_fd, framebuffer_addr, shop_head, goods_head, bm, f, (2 + (page_flag - 1) * 6));
                goto back_from_cart;
            }
            else if ((x > 405 && x < 540) && (y > 68 && y < 190))
            {
                if ((3 + (page_flag - 1) * 6) > total_cnt)
                {
                    continue;
                }
                ctrl_the_details_page(ts_fd, framebuffer_addr, shop_head, goods_head, bm, f, (3 + (page_flag - 1) * 6));
                goto back_from_cart;
            }
            else if ((x > 45 && x < 190) && (y > 275 && y < 390))
            {
                if ((4 + (page_flag - 1) * 6) > total_cnt)
                {
                    continue;
                }
                ctrl_the_details_page(ts_fd, framebuffer_addr, shop_head, goods_head, bm, f, (4 + (page_flag - 1) * 6));
                goto back_from_cart;
            }
            else if ((x > 225 && x < 365) && (y > 275 && y < 390))
            {
                if ((5 + (page_flag - 1) * 6) > total_cnt)
                {
                    continue;
                }
                ctrl_the_details_page(ts_fd, framebuffer_addr, shop_head, goods_head, bm, f, (5 + (page_flag - 1) * 6));
                goto back_from_cart;
            }
            else if ((x > 405 && x < 540) && (y > 275 && y < 390))
            {
                if ((6 + (page_flag - 1) * 6) > total_cnt)
                {
                    continue;
                }
                ctrl_the_details_page(ts_fd, framebuffer_addr, shop_head, goods_head, bm, f, (6 + (page_flag - 1) * 6));
                goto back_from_cart;
            }
            else if ((x > 0 && x < 30) && (y > 0 && y < 30))
            {
                if (flag < 0)
                {
                    sort_list(goods_head);
                    flag = -flag;
                }
                else if (flag > 0)
                {
                    start_input_goods_list(goods_head);
                    goods_t *pos;
                    list_for_each_entry(pos, &goods_head->list, list)
                    {
                        printf("%s\n", pos->name);
                    }
                    flag = -flag;
                }
                display_jpeg_format_to_framebuffer("红向上箭头.jpg", 0, 0, framebuffer_addr, 1);
                usleep(50000);
                display_jpeg_format_to_framebuffer("黄向上箭头.jpg", 0, 0, framebuffer_addr, 1);
                goto back_from_cart;
            }
            else if ((x > 30 && x < 60) && (y > 0 && y < 30))
            {
                if (flag < 0)
                {
                    sort_list_down(goods_head);
                    flag = -flag;
                }
                else if (flag > 0)
                {
                    start_input_goods_list(goods_head);
                    goods_t *pos;
                    list_for_each_entry(pos, &goods_head->list, list)
                    {
                        printf("%s\n", pos->name);
                    }
                    flag = -flag;
                }
                display_jpeg_format_to_framebuffer("红向下箭头.jpg", 30, 0, framebuffer_addr, 1);
                usleep(50000);
                display_jpeg_format_to_framebuffer("黄向下箭头.jpg", 30, 0, framebuffer_addr, 1);
                goto back_from_cart;
            }
            else if ((x > 750 && x < 800) && (y > 0 && y < 50))
            {
                int specfic_page;
                close_touchscreen_device(ts_fd);
                specfic_page = search_the_goods(goods_head);
                ts_fd = open_touchscreen_device();
                if (specfic_page == -1)
                {
                    display_jpeg_format_to_framebuffer("找不到.jpg", 300, 140, framebuffer_addr, 1);
                    sleep(1);
                    goto back_from_cart;
                }
                else
                {
                    ctrl_the_details_page(ts_fd, framebuffer_addr, shop_head, goods_head, bm, f, specfic_page);
                    goto back_from_cart;
                }
            }
            else if ((x > 375 && x < 425) && (y > 0 && y < 50)) //进入商品管理界面
            {
                secret_num++;
                if (secret_num == 7)
                {
                    display_jpeg_format_to_framebuffer("管理员界面.jpg", 0, 0, framebuffer_addr, 1);
                    secret_num = 0;
                    ctrl_admin_page(goods_head, ts_fd);
                    goto back_from_cart;
                }
            }
        }
    }
    return 0;
}

//控制购物车画面
int ctrl_the_shopping_cart(int ts_fd, unsigned int *framebuffer_addr, shop_t *shop_head, goods_t *goods_head, bitmap *bm, font *f, unsigned int page_flag)
{

    int x, y;
    int status;
    int retval;
    shop_t *temp;
    int total_kinds;
    int total_page;
    total_kinds = foreach_shopping_cart(shop_head);
    total_page = total_kinds / 4;
    if (total_kinds % 4 != 0)
    {
        total_page++;
    }
    printf("total_page = %d\n", total_page);
    if (total_page < page_flag && total_page != 0)
    {
        //显示没有更多页了
        printf("当前已经是最后一页了\n");
        return 2;
    }
back_from_former_page:
    display_shopping_cart_page(shop_head, goods_head, bm, f, page_flag, framebuffer_addr);
    while (1)
    {
        status = get_touchscreen_coordinate(ts_fd, &x, &y);
        if ((x > 665 && x < 700) && (y > 70 && y < 140) && status == TOUCHSCREEN_TOUCH_STATUS)
        {
            return 0; //正常结束
        }
        else if ((x > 175 && x < 240) && (y > 105 && y < 135) && status == TOUCHSCREEN_RELEASE_STATUS)
        {
            clear_shopping_cart(shop_head);
            display_shopping_cart_page(shop_head, goods_head, bm, f, 1, framebuffer_addr);
        }
        else if ((x > 290 && x < 350) && (y > 330 && y < 370) && (page_flag > 1) && status == TOUCHSCREEN_RELEASE_STATUS) //上一页
        {
            return 1; //从此页返回上一页
        }
        else if ((x > 390 && x < 450) && (y > 330 && y < 370) && (page_flag >= 1) && status == TOUCHSCREEN_RELEASE_STATUS) //下一页
        {
            retval = ctrl_the_shopping_cart(ts_fd, framebuffer_addr, shop_head, goods_head, bm, f, page_flag + 1);
            if (retval == 2) //防止下一层返回时意外结束程序
            {
                continue;
            }
            else if (retval == 1)
            {
                goto back_from_former_page;
            }
            else
            {
                break;
            }
        }
        else if ((x > 430 && x < 465) && (y > 155 && y < 190) && (page_flag >= 1) && status == TOUCHSCREEN_RELEASE_STATUS && 1 * page_flag <= total_kinds) //第一行add
        {
            ADD_GOODS_COUNT_IN_CART(1, page_flag, shop_head, goods_head, framebuffer_addr); //宏函数避免太大块
        }
        else if ((x > 430 && x < 465) && (y > 195 && y < 230) && (page_flag >= 1) && status == TOUCHSCREEN_RELEASE_STATUS && 2 * page_flag <= total_kinds) //第二行add
        {
            ADD_GOODS_COUNT_IN_CART(2, page_flag, shop_head, goods_head, framebuffer_addr); //宏函数避免太大块
        }
        else if ((x > 430 && x < 465) && (y > 235 && y < 270) && (page_flag >= 1) && status == TOUCHSCREEN_RELEASE_STATUS && 3 * page_flag <= total_kinds) //第三行add
        {
            ADD_GOODS_COUNT_IN_CART(3, page_flag, shop_head, goods_head, framebuffer_addr); //宏函数避免太大块
        }
        else if ((x > 430 && x < 465) && (y > 275 && y < 310) && (page_flag >= 1) && status == TOUCHSCREEN_RELEASE_STATUS && 4 * page_flag <= total_kinds) //第四行add
        {
            ADD_GOODS_COUNT_IN_CART(4, page_flag, shop_head, goods_head, framebuffer_addr); //宏函数避免太大块
        }
        else if ((x > 510 && x < 555) && (y > 155 && y < 190) && (page_flag >= 1) && status == TOUCHSCREEN_RELEASE_STATUS && 1 * page_flag <= total_kinds) //第一行dec
        {
            DEC_GOODS_COUNT_IN_CART(1, goods_head, shop_head, page_flag, framebuffer_addr);
        }
        else if ((x > 510 && x < 555) && (y > 195 && y < 230) && (page_flag >= 1) && status == TOUCHSCREEN_RELEASE_STATUS && 2 * page_flag <= total_kinds) //第二行dec
        {
            DEC_GOODS_COUNT_IN_CART(2, goods_head, shop_head, page_flag, framebuffer_addr); //宏函数避免太大块
        }
        else if ((x > 510 && x < 555) && (y > 235 && y < 270) && (page_flag >= 1) && status == TOUCHSCREEN_RELEASE_STATUS && 3 * page_flag <= total_kinds) //第三行dec
        {
            DEC_GOODS_COUNT_IN_CART(3, goods_head, shop_head, page_flag, framebuffer_addr); //宏函数避免太大块
        }
        else if ((x > 510 && x < 555) && (y > 275 && y < 310) && (page_flag >= 1) && status == TOUCHSCREEN_RELEASE_STATUS && 4 * page_flag <= total_kinds) //第四行dec
        {
            DEC_GOODS_COUNT_IN_CART(4, goods_head, shop_head, page_flag, framebuffer_addr); //宏函数避免太大块
        }
        else if ((x > 140 && x < 216) && (y > 330 && y < 360) && (page_flag >= 1) && status == TOUCHSCREEN_RELEASE_STATUS && total_kinds > 0) //付款
        {
            int ret = ctrl_the_check_out(ts_fd, framebuffer_addr, shop_head, goods_head, bm, f, 1);
            if (ret == 0)
            {
                goto back_from_former_page;
            }
            clear_shopping_cart(shop_head);
            return 7;
        }
    }
    return 0;
}

//控制付款页面
int ctrl_the_check_out(int ts_fd, unsigned int *framebuffer_addr, shop_t *shop_head, goods_t *goods_head, bitmap *bm, font *f, unsigned int page_flag)
{
    int x, y;
    int status;
    int retval;
    shop_t *temp;
    int total_kinds;
    int total_page;
    total_kinds = foreach_shopping_cart(shop_head);
    total_page = total_kinds / 4;
    if (total_kinds % 4 != 0)
    {
        total_page++;
    }
    printf("total_page = %d\n", total_page);
    if (total_page < page_flag && total_page != 0)
    {
        //显示没有更多页了
        printf("当前已经是最后一页了\n");
        return 2;
    }
back_from_former_page:
    display_check_out_page(shop_head, goods_head, bm, f, page_flag, framebuffer_addr);
    while (1)
    {
        status = get_touchscreen_coordinate(ts_fd, &x, &y);
        if ((x > 665 && x < 700) && (y > 70 && y < 140) && status == TOUCHSCREEN_TOUCH_STATUS)
        {
            return 0; //正常结束
        }
        else if ((x > 290 && x < 350) && (y > 330 && y < 370) && (page_flag > 1) && status == TOUCHSCREEN_RELEASE_STATUS) //上一页
        {
            // ctrl_the_shopping_cart(ts_fd, framebuffer_addr, shop_head, goods_head, bm, f, page_flag - 1);
            return 1; //从此页返回上一页
        }
        else if ((x > 390 && x < 450) && (y > 330 && y < 370) && (page_flag >= 1) && status == TOUCHSCREEN_RELEASE_STATUS) //下一页
        {
            retval = ctrl_the_check_out(ts_fd, framebuffer_addr, shop_head, goods_head, bm, f, page_flag + 1);
            if (retval == 2) //防止下一层返回时意外结束程序
            {
                continue;
            }
            else if (retval == 1)
            {
                goto back_from_former_page;
            }
            else
            {
                break;
            }
        }
        else if ((x > 120 && x < 190) && (y > 330 && y < 370) && (page_flag >= 1) && status == TOUCHSCREEN_RELEASE_STATUS)
        {
            display_jpeg_format_to_framebuffer("收款码.jpg", 250, 120, framebuffer_addr, 1);
            sleep(3);
            display_jpeg_format_to_framebuffer("付款成功.jpg", 0, 0, framebuffer_addr, 1);
            sleep(3);
            start_input_goods_list(goods_head);
            update_goods_stock_info(goods_head, shop_head);
            return 1;
        }
    }
    return 0;
}

//控制商品详情页
int ctrl_the_details_page(int ts_fd, unsigned int *framebuffer_addr, shop_t *shop_head, goods_t *goods_head, bitmap *bm, font *f, unsigned int page_flag)
{
    int x, y;
    int status;
    int retval;
    goods_t *temp;
    int total_page;
    total_page = total_number_of_goods(goods_head);

    printf("total_page = %d\n", total_page);
    if (total_page < page_flag && total_page != 0)
    {
        //显示没有更多页了
        printf("当前已经是最后一页了\n");
        return 2;
    }
back_from_former_page:
    display_goods_detail_page(shop_head, goods_head, bm, f, page_flag, framebuffer_addr);
    temp = serach_specific_goods_in_orders(goods_head, page_flag);
    while (1)
    {
        status = get_touchscreen_coordinate(ts_fd, &x, &y);
        if (status == TOUCHSCREEN_RELEASE_STATUS)
        {
            if ((x > 670 && x < 780) && (y > 0 && y < 90)) // 退出
            {
                return 0;
            }
            else if ((x > 380 && x < 544) && (y > 390 && y < 435) && temp->stock > 0)
            {
                add_goods_in_shopping_cart(1, page_flag, shop_head, goods_head);
                display_jpeg_format_to_framebuffer("亮详情车.jpg", 420, 390, framebuffer_addr, 1);
                usleep(50000);
                display_jpeg_format_to_framebuffer("详情车.jpg", 420, 390, framebuffer_addr, 1);
            }
            else if ((x > 6 && x < 240) && (y > 200 && y < 280) && page_flag > 1)
            {
                retval = ctrl_the_details_page(ts_fd, framebuffer_addr, shop_head, goods_head, bm, f, page_flag - 1);
                if (retval == 1)
                {
                    goto back_from_former_page;
                }
                else if (retval == 0)
                {
                    return 0;
                }
            }
            else if ((x > 728 && x < 790) && (y > 200 && y < 280) && page_flag < total_page)
            {
                retval = ctrl_the_details_page(ts_fd, framebuffer_addr, shop_head, goods_head, bm, f, page_flag + 1);
                if (retval == 1)
                {
                    goto back_from_former_page;
                }
                else if (retval == 0)
                {
                    return 0;
                }
            }
        }
    }
}

//欢迎界面
int ctrl_welcome_page(int ts_fd, unsigned int *framebuffer_addr)
{
    int status;
    int x, y;
    display_jpeg_format_to_framebuffer("欢迎界面.jpg", 0, 0, framebuffer_addr, 1);
    while (1)
    {
        status = get_touchscreen_coordinate(ts_fd, &x, &y);
        if (status == TOUCHSCREEN_RELEASE_STATUS)
        {
            break;
        }
    }
    return 0;
}

//商品添加商品
int ctrl_admin_page(goods_t *goods_head, int ts_fd)
{
    start_input_goods_list(goods_head);//重新初始化链表
    char *buffer;
    int id;
    int stock;
    int x, y;
    int status;
    float price;
    static int limit_id = 15;
    printf("输入新商品的数字ID\n");
    scanf("%d", &id);
    while (id < limit_id)
    {
        printf("非法ID，请重新输入,ID必须大于%d\n", limit_id - 1);
        scanf("%d", &id);
    }
    limit_id++;
    buffer = (char *)malloc(100);
    printf("请输入商品名(商品名小于20字节)\n");
    scanf("%s", buffer);
    while (strlen(buffer) > 19)
    {
        printf("非法商品名，请重新输入\n");
        scanf("%s", buffer);
    }
    printf("请输入商品库存(库存数必须大于0)\n");
    scanf("%d", &stock);
    while (stock <= 0)
    {
        printf("我就猜到你会乱输，请重新输入\n");
        scanf("%d", &stock);
    }
    printf("请输入商品价格(价格必须大于0)\n");
    scanf("%f", &price);
    while (price <= 0)
    {
        printf("非法价格，请重新输入\n");
        scanf("%d", &price);
    }
    goods_info_add(goods_head, id, buffer, price, stock);
    output_goods_info_to_file(goods_head);//输出链表信息到文件中
    while (1)
    {
        status = get_touchscreen_coordinate(ts_fd, &x, &y);
        if (status == TOUCHSCREEN_RELEASE_STATUS)
        {
            if ((x > 700 && x < 800) && (y > 0 && y < 50))
            {
                break;
            }
        }
    }
    free(buffer);
    return 0;
}