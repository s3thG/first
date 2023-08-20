#include <stdio.h>
#include <stdlib.h>
#include <list.h>
#include <goods_info.h>
#include <shopping_cart.h>
#include <string.h>

//申请新的购物车节点
shop_t *request_new_shop_cart_node(void)
{
    shop_t *new_node = (shop_t *)malloc(sizeof(shop_t));
    if (new_node == NULL)
    {
        perror("申请节点内存失败");
        return new_node;
    }
    return new_node;
}

//销毁节点
void destory_cart_node(shop_t *node)
{
    free(node);
}

//将商品信息加入到购物车
int add_goods_in_shopping_cart(int goods_cnt, int goods_order, shop_t *s_head_node, goods_t *goods_head)
{
    int goods_id = in_order_return_goods_id(goods_head,goods_order);
    int retval = serach_same_goods_in_cart(s_head_node, goods_id);
    if (retval != -1)
    {
        change_goods_count_in_cart(1, goods_id, s_head_node, goods_head);
        return 0;
    }
    goods_t *spec_goods = serach_specific_goods(goods_head, goods_id); //找到对应ID的商品
    shop_t *new_node = request_new_shop_cart_node();
    new_node->goods_id = goods_id;
    if (goods_cnt > spec_goods->stock)
    {
        return -1;
    }
    else
    {
        new_node->purchase_cnt = goods_cnt;
    }
    new_node->purchase_cnt = goods_cnt;
    strcpy(new_node->name, spec_goods->name);
    new_node->price = spec_goods->price;
    list_add_tail(&new_node->s_list, &s_head_node->s_list);
    return 0;
}

//将商品信息加入到购物车
int add_goods_in_shopping_cart2(int goods_cnt, int goods_id, shop_t *s_head_node, goods_t *goods_head)
{
    int retval = serach_same_goods_in_cart(s_head_node, goods_id);
    if (retval != -1)
    {
        change_goods_count_in_cart(1, goods_id, s_head_node, goods_head);
        return 0;
    }
    goods_t *spec_goods = serach_specific_goods(goods_head, goods_id); //找到对应ID的商品
    shop_t *new_node = request_new_shop_cart_node();
    new_node->goods_id = goods_id;
    if (goods_cnt > spec_goods->stock)
    {
        return -1;
    }
    else
    {
        new_node->purchase_cnt = goods_cnt;
    }
    new_node->purchase_cnt = goods_cnt;
    strcpy(new_node->name, spec_goods->name);
    new_node->price = spec_goods->price;
    list_add_tail(&new_node->s_list, &s_head_node->s_list);
    return 0;
}

//修改购物车内商品数量
void change_goods_count_in_cart(int goods_cnt, unsigned int id, shop_t *s_head_node, goods_t *goods_head)
{
    shop_t *pos;
    int retval;
    list_for_each_entry(pos, &s_head_node->s_list, s_list)
    {
        if (pos->goods_id == id)
        {
            pos->purchase_cnt += goods_cnt;
            while (stock_info_preview(goods_head, pos->goods_id, pos->purchase_cnt))
            {
                if (pos->purchase_cnt <= 0)
                {
                    pos->purchase_cnt = 0;
                    break;
                }
                pos->purchase_cnt--;
            }
            break;
        }
    }
}

//清空购物车
void clear_shopping_cart(shop_t *s_head_node)
{
    shop_t *pos, *n;
    list_for_each_entry_safe(pos, n, &s_head_node->s_list, s_list)
    {
        list_del(&pos->s_list);
        destory_cart_node(pos);
    }
    printf("购物车已清空\n");
}

//删除购物车内特定商品
void del_cart_specific_goods(unsigned int id, shop_t *s_head_node)
{
    shop_t *pos, *n;
    list_for_each_entry_safe(pos, n, &s_head_node->s_list, s_list)
    {
        if (pos->goods_id == id)
        {
            list_del(&pos->s_list);
            destory_cart_node(pos);
            break;
        }
    }
}

//统计购物车商品品种总数
int foreach_shopping_cart(shop_t *s_head_node)
{
    shop_t *pos;
    int cnt = 0;
    //先统计购物车内商品的种类总数
    list_for_each_entry(pos, &s_head_node->s_list, s_list)
    {
        cnt++;
    }
    return cnt;
}

//购物车价格总计
float whole_cart_cost_sum(shop_t *s_head_node)
{
    shop_t *pos;
    float sum = 0.0;
    list_for_each_entry(pos, &s_head_node->s_list, s_list)
    {
        sum += pos->price * pos->purchase_cnt;
    }
    return sum;
}

//确认购物车是否有同样的商品
int serach_same_goods_in_cart(shop_t *s_head_node, unsigned int id)
{
    shop_t *pos;
    list_for_each_entry(pos, &s_head_node->s_list, s_list)
    {
        if (pos->goods_id == id)
        {
            return id;
        }
    }
    return -1;
}

//根据商品在购物车的次序搜索商品，返回地址
shop_t *get_pos_at_shopping_list(int idx, shop_t *s_head_node)
{
    shop_t *pos;
    int cnt = 0;
    list_for_each_entry(pos, &s_head_node->s_list, s_list)
    {
        cnt++;
        if (cnt == idx)
        {
            return pos;
        }
    }
    return NULL;
}

//更新商品库存信息
int update_goods_stock_info(goods_t *goods_head, shop_t *shop_head)
{
    goods_t *pos;
    shop_t *s_pos;
    list_for_each_entry(s_pos, &shop_head->s_list, s_list)
    {
        pos = serach_specific_goods(goods_head, s_pos->goods_id);
        printf("找到商品：%s\n", pos->name);
        pos->stock -= s_pos->purchase_cnt;
    }
    return 0;
}
