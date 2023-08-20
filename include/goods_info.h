#ifndef __GOODS_INFO_h__
#define __GOODS_INFO_h__

#include <stdio.h>
#include <list.h>

//定义商品结构体信息，使用内核链表实现
typedef struct goods_list {
    char name[20];  //商品名    
    float price;    //价格
    int stock;      //库存
    unsigned int goods_id;  //商品编号
    struct list_head list;
}goods_t;

extern goods_t *request_new_node(void);
extern int goods_info_add(goods_t *head_node, unsigned int id, const char *name, float price, int stock);
extern int output_goods_info_to_file(goods_t *head_node);
extern int start_input_goods_list(goods_t *head_node);
extern void manual_input_goods_info(goods_t *head_node);
extern goods_t *serach_specific_goods(goods_t *head_node, unsigned int order);
extern goods_t *serach_specific_goods_in_orders(goods_t *head_node, unsigned int order);
extern int total_number_of_goods(goods_t *head_node);
extern int stock_info_preview(goods_t *head_node, unsigned int id, int stock_chage);
extern unsigned int in_order_return_goods_id(goods_t *head_node,int order);
extern void sort_list(goods_t *head);
extern void sort_list_down(goods_t *head);
extern int search_the_goods(goods_t *head_node);

#endif