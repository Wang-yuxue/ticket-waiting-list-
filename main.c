#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 1. 定义订单信息
typedef struct {
    char orderId[50];  // 订单号
    char name[50];     // 姓名
    char idCard[50];   // 身份证号
} Order;

// 2. 定义队列节点（链表节点）
typedef struct QueueNode {
    Order data;
    struct QueueNode* next;
} QueueNode;

// 3. 定义链式队列
typedef struct {
    QueueNode* front;  // 队头指针（出队）
    QueueNode* rear;   // 队尾指针（入队）
} LinkedQueue;

// ---------- 队列基础操作 ----------
// 初始化队列
void initQueue(LinkedQueue* q) {
    q->front = NULL;
    q->rear = NULL;
}

// 判断队列是否为空
int isEmpty(LinkedQueue* q) {
    return q->front == NULL;
}

// 入队操作
void enqueue(LinkedQueue* q, Order order) {
    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    if (newNode == NULL) {
        printf("内存分配失败！\n");
        return;
    }
    newNode->data = order;
    newNode->next = NULL;

    if (isEmpty(q)) {
        q->front = newNode;
        q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
}

// 出队操作
int dequeue(LinkedQueue* q, Order* outOrder) {
    if (isEmpty(q)) {
        return 0; // 失败
    }
    QueueNode* temp = q->front;
    *outOrder = temp->data;
    q->front = q->front->next;

    if (q->front == NULL) {
        q->rear = NULL; // 队列已空
    }
    free(temp); // 释放内存
    return 1; // 成功
}

// 清空队列（释放剩余内存）
void clearQueue(LinkedQueue* q) {
    Order temp;
    while (!isEmpty(q)) {
        dequeue(q, &temp);
    }
}

// ---------- 业务功能 ----------
// 功能1：新增候补订单（入队）
void addOrder(LinkedQueue* q) {
    Order newOrder;
    printf("请输入订单号: ");
    scanf("%s", newOrder.orderId);
    printf("请输入旅客姓名: ");
    scanf("%s", newOrder.name);
    printf("请输入身份证号: ");
    scanf("%s", newOrder.idCard);

    // 加分项：防重校验（每人只能有一张候补票）
    QueueNode* curr = q->front;
    while (curr != NULL) {
        if (strcmp(curr->data.idCard, newOrder.idCard) == 0) {
            printf("【提示】该身份证号已在候补队列中，不可重复提交！\n");
            return;
        }
        curr = curr->next;
    }

    enqueue(q, newOrder);
    printf("【成功】候补订单已提交，正在排队中...\n");
}

// 功能2：余票发放（出队）
void releaseTickets(LinkedQueue* q) {
    if (isEmpty(q)) {
        printf("【提示】当前没有候补订单，无需发放。\n");
        return;
    }

    int k;
    printf("请输入本次释放的余票数量 k: ");
    if (scanf("%d", &k) != 1 || k <= 0) {
        printf("输入无效！余票数量必须为大于0的整数。\n");
        while (getchar() != '\n'); // 清空输入缓冲区
        return;
    }

    printf("\n--- 开始分配余票 ---\n");
    Order successOrder;
    int successCount = 0;

    // 核心逻辑：只要有余票，且队列不为空，就持续出队
    while (k > 0 && !isEmpty(q)) {
        dequeue(q, &successOrder);
        // 因为每人只买一张票，所以余票每次减1
        printf("候补成功！分配车票 -> 订单号: %s | 姓名: %s | 身份证: %s\n",
               successOrder.orderId, successOrder.name, successOrder.idCard);
        k--;
        successCount++;
    }

    printf("--- 分配结束 ---\n");
    // 状态提示
    if (k == 0 && !isEmpty(q)) {
        printf("【提示】余票已发完，队列中还有剩余候补旅客，请继续等待。\n");
    } else if (k > 0 && isEmpty(q)) {
        printf("【提示】候补队列已清空，本次剩余 %d 张余票未发完。\n", k);
    }
}

// ---------- 主函数 ----------
int main() {
    LinkedQueue waitList;
    initQueue(&waitList);

    int choice;
    while (1) {
        printf("1. 新增候补订单\n");
        printf("2. 余票发放\n");
        printf("3. 退出程序\n");
        printf("请输入操作编号: ");

        if (scanf("%d", &choice) != 1) {
            printf("输入错误，请输入数字！\n");
            while (getchar() != '\n'); // 清空缓冲区防止死循环
            continue;
        }

        switch (choice) {
            case 1:
                addOrder(&waitList);
                break;
            case 2:
                releaseTickets(&waitList);
                break;
            case 3:
                printf("程序已退出，感谢使用！\n");
                clearQueue(&waitList); // 退出前释放内存，防止内存泄漏
                return 0;
            default:
                printf("无效的操作编号，请重新输入！\n");
        }
    }
    return 0;
}
