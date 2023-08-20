#include <stdio.h>
#include <stdlib.h>
#include <list.h>
#include <errno.h>
#include <goods_info.h>
#include <string.h>
#include <shopping_cart.h>

#define SIZE 1024

//申请链表节点
goods_t *request_new_node(void)
{
    goods_t *new_node = (goods_t *)malloc(sizeof(goods_t));
    if (new_node == NULL)
    {
        perror("申请节点内存失败");
        return new_node;
    }
    return new_node;
}

//销毁节点
void destory_goods_node(goods_t *node)
{
    free(node);
}

//商品信息录入
int goods_info_add(goods_t *head_node, unsigned int id, const char *name, float price, int stock)
{
    goods_t *new_node;
    new_node = request_new_node();
    if (new_node == NULL)
    {
        perror("申请节点内存失败");
        return -1;
    }
    new_node->goods_id = id;
    strcpy(new_node->name, name);
    new_node->price = price;
    new_node->stock = stock;
    list_add_tail(&new_node->list, &head_node->list);
    return 0;
}


//将链表信息保存到文件中
int output_goods_info_to_file(goods_t *head_node)
{
    FILE *fp;
    goods_t *pos, *n;
    fp = fopen("goods_list.txt", "w");
    if (fp == NULL)
    {
        perror("打开文件流指针失败");
        return -1;
    }
    //遍历大结构体，格式化写入到商品链表文件中
    list_for_each_entry_safe(pos, n, &head_node->list, list)
    {
        fprintf(fp, "%u ", pos->goods_id); //输出商品编号
        fprintf(fp, "%s ", pos->name);     //输出商品信息
        fprintf(fp, "%f ", pos->price);    //输出商品价格
        fprintf(fp, "%d ", pos->stock);    //输出商品库存
        fprintf(fp, "\n");
    }
    fclose(fp);
    return 0;
}

//从商品信息文件加载信息到链表中
int start_input_goods_list(goods_t *head_node)
{
    clear_goods_info(head_node);
    FILE *fp;
    fp = fopen("goods_list.txt", "r");
    char *buffer;
    buffer = (char *)malloc(SIZE);
    int i = 0;
    while (fgets(buffer, SIZE, fp) != NULL)
    {
        printf("%d", i++);
        goods_t *new_node = request_new_node();
        sscanf(buffer, "%u %s %f %d", &new_node->goods_id, new_node->name, &new_node->price, &new_node->stock);
        list_add_tail(&new_node->list, &head_node->list);
    }
    fclose(fp);
    free(buffer);
    return 0;
}

//查找商品 返回对应商品指针
goods_t *serach_specific_goods(goods_t *head_node, unsigned int goods_id)
{
    goods_t *pos;
    // int cnt = 0;
    list_for_each_entry(pos, &head_node->list, list)
    {
        // cnt++;
        //  if (cnt == order)
        //  {
        //      printf("找到目标商品\n");
        //      return pos;
        //  }
        if (goods_id == pos->goods_id)
        {
            printf("找到商品：%s\n", pos->name);
            return pos;
        }
    }
    if (&pos->list == &head_node->list)
    {
        printf("找不到商品\n");
        return NULL;
    }
}

//按次序查找
goods_t *serach_specific_goods_in_orders(goods_t *head_node, unsigned int order)
{
    goods_t *pos;
    int cnt = 0;
    list_for_each_entry(pos, &head_node->list, list)
    {
        cnt++;
        if (cnt == order)
        {
            printf("找到目标商品\n");
            return pos;
        }
        // if(goods_id == pos->goods_id)
        // {
        //     printf("找到商品：%s\n",pos->name);
        //     return pos;
        // }
    }
    if (&pos->list == &head_node->list)
    {
        printf("找不到商品\n");
        return NULL;
    }
}

//商品出库前查询库存
int stock_info_preview(goods_t *head_node, unsigned int id, int stock_chage)
{
    goods_t *pos;
    list_for_each_entry(pos, &head_node->list, list)
    {
        if (pos->goods_id == id)
        {
            printf("找到目标商品\n");
            if (pos->stock - (stock_chage) < 0)
            {
                printf("%s库存不足,请修改购买数量\n", pos->name);
                //返回不能购买指令到用户界面
                return -1;
            }
            else
            {
                printf("库存充足\n");
                break;
            }
        }
    }
    return 0;
}
//更新商品库存信息
// int update_goods_info(goods_t *head_node, unsigned int id, int stock_chage)
// {
//     goods_t *pos;
//     list_for_each_entry(pos, &head_node->list, list)
//     {
//         if (pos->goods_id == id)
//         {
//             printf("找到目标商品\n");
//             pos->stock -= stock_chage;
//         }
//     }
// }

//统计当前商品数量
int total_number_of_goods(goods_t *head_node)
{
    goods_t *pos;
    int cnt = 0;
    list_for_each_entry(pos, &head_node->list, list)
    {
        cnt++;
    }
    return cnt;
}

//按次序返回id
unsigned int in_order_return_goods_id(goods_t *head_node, int order)
{
    goods_t *pos;
    int cnt = 0;
    list_for_each_entry(pos, &head_node->list, list)
    {
        cnt++;
        if (cnt == order)
        {
            printf("%s加入购物车\n", pos->name);
            return pos->goods_id;
        }
    }
    return -1;
}

//根据价格从高到低对商品链表排序
void sort_list(goods_t *head)
{
    struct list_head *pos, *n; //用小结构体实现
    goods_t temp;
    goods_t *npos;
    list_for_each_prev(pos, &head->list) // pos向上遍历，这样就能每次少一个尾巴
    {
        for (n = (&head->list)->next; n != pos; n = n->next)
        {
            if (list_entry(n, goods_t, list)->price > list_entry(n->next, goods_t, list)->price)
            {
                // id交换
                temp.goods_id = list_entry(n, goods_t, list)->goods_id;
                list_entry(n, goods_t, list)->goods_id = list_entry(n->next, goods_t, list)->goods_id;
                list_entry(n->next, goods_t, list)->goods_id = temp.goods_id;
                //名字交换
                strcpy(temp.name, list_entry(n, goods_t, list)->name);
                strcpy(list_entry(n, goods_t, list)->name, list_entry(n->next, goods_t, list)->name);
                strcpy(list_entry(n->next, goods_t, list)->name, temp.name);
                //价格交换
                temp.price = list_entry(n, goods_t, list)->price;
                list_entry(n, goods_t, list)->price = list_entry(n->next, goods_t, list)->price;
                list_entry(n->next, goods_t, list)->price = temp.price;
                //交换库存
                temp.stock = list_entry(n, goods_t, list)->stock;
                list_entry(n, goods_t, list)->stock = list_entry(n->next, goods_t, list)->stock;
                list_entry(n->next, goods_t, list)->stock = temp.stock;
            }
        }
    }
    list_for_each_entry(npos, &head->list, list)
    {
        printf("%s\n", npos->name);
    }
}

void sort_list_down(goods_t *head)
{
    struct list_head *pos, *n; //用小结构体实现
    goods_t temp;
    goods_t *npos;
    list_for_each_prev(pos, &head->list) // pos向上遍历，这样就能每次少一个尾巴
    {
        for (n = (&head->list)->next; n != pos; n = n->next)
        {
            if (list_entry(n, goods_t, list)->price < list_entry(n->next, goods_t, list)->price)
            {
                // id交换
                temp.goods_id = list_entry(n, goods_t, list)->goods_id;
                list_entry(n, goods_t, list)->goods_id = list_entry(n->next, goods_t, list)->goods_id;
                list_entry(n->next, goods_t, list)->goods_id = temp.goods_id;
                //名字交换
                strcpy(temp.name, list_entry(n, goods_t, list)->name);
                strcpy(list_entry(n, goods_t, list)->name, list_entry(n->next, goods_t, list)->name);
                strcpy(list_entry(n->next, goods_t, list)->name, temp.name);
                //价格交换
                temp.price = list_entry(n, goods_t, list)->price;
                list_entry(n, goods_t, list)->price = list_entry(n->next, goods_t, list)->price;
                list_entry(n->next, goods_t, list)->price = temp.price;
                //交换库存
                temp.stock = list_entry(n, goods_t, list)->stock;
                list_entry(n, goods_t, list)->stock = list_entry(n->next, goods_t, list)->stock;
                list_entry(n->next, goods_t, list)->stock = temp.stock;
            }
        }
    }
    list_for_each_entry(npos, &head->list, list)
    {
        printf("%s\n", npos->name);
    }
}

//按名字搜索商品,返回次序
int search_the_goods(goods_t *head_node)
{
    char *buffer;
    goods_t *pos;
    int cnt;
    buffer = (char *)malloc(100);
    printf("请输入要查找的商品名：\n");
    scanf("%s",buffer);
    cnt = 0;
    list_for_each_entry(pos, &head_node->list, list)
    {
        cnt++;
        if (strcmp(buffer, pos->name) == 0)
        {
            printf("找到商品%s\n", pos->name);
            return  cnt;
        }
    }
    printf("找不到你要的商品\n");
    return -1;
}

//清空链表
void clear_goods_info(goods_t *head_node)
{
    goods_t *pos, *n;
    list_for_each_entry_safe(pos, n, &head_node->list, list)
    {
        list_del(&pos->list);
        free(pos);
    }
}