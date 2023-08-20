#ifndef __SHOPPING_CART_h__
#define __SHOPPING_CART_h__

#include <stdio.h>
#include <list.h>

//定义购物车结构体信息，使用内核链表实现
typedef struct shopping_cart {
    char name[20];
    unsigned int goods_id;
    int purchase_cnt;
    float price;
    struct list_head s_list;
}shop_t;

extern shop_t *request_new_shop_cart_node(void);
extern void destory_cart_node(shop_t *node);
extern int add_goods_in_shopping_cart(int goods_cnt, int goods_order, shop_t *s_head_node, goods_t *goods_head);
extern void change_goods_count_in_cart(int goods_cnt, unsigned int id, shop_t *s_head_node,goods_t *goods_head);
extern void clear_shopping_cart(shop_t *s_head_node);
extern void del_cart_specific_goods(unsigned int id, shop_t *s_head_node);
extern int foreach_shopping_cart(shop_t *s_head_node);
extern float whole_cart_cost_sum(shop_t *s_head_node);
extern int serach_same_goods_in_cart(shop_t * s_head_node,unsigned int id);
extern shop_t *get_pos_at_shopping_list(int idx, shop_t *s_head_node);
extern int update_goods_stock_info(goods_t *goods_head, shop_t *shop_head);

#endif