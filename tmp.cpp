/*
* 作者：
功能：模拟进程状态转换（进程数范围：1-10）
4个PCB状态标识常量：1.就绪Ready、2.运行Running、3.阻塞Block、4.终止Terminate
用Run（）函数去模拟的CPU的run：所以Run函数应包含以下情况的处理（即，4种队列对应的处理情况）：
		（1）就绪队列（为了方便找最大优先级，当然用priority_queue更方便，但我采用更熟悉的链表结构）：
			就绪_1->就绪_1
			就绪_1->运行_2

		（2）运行队列
			运行_2->就绪_1
			运行_2->运行_2
			运行_2->阻塞_3
			运行_2->终止_4
		
		（3）阻塞队列（唤醒概率：1/8、被阻塞概率:1/4）
			阻塞_3->唤醒->就绪_1
			阻塞_3->唤醒->运行_1，（当就绪、运行队列为空时的情况，跳过进入就绪，直接唤醒到运行）
			阻塞_3->阻塞_3//唤醒概率没有满足的时候

		（4）终止队列：
			对终止队列里的进程不必要进行处理
*/

#include <iostream>
#include <cstdlib>
#include<queue>
#include <windows.h>
using namespace std;
enum State {
	Ready,Running,BlockForIO,Terminate
};
struct PCB {
	int id = 0;
	int priority = 0;//初始化进程时，随机分配,抢占式优先级分配
	int timeslice = 0;//初始化进程时，随机分配
	State state = Ready;//假设各进程满足除CPU外一切资源，初始状态为就绪
	PCB* next=NULL;
};
PCB* head=NULL;
queue<PCB*> runningQueue, blockQueueForIO, terminateQueue;
int blockedProbability;
void InitPCB();
PCB* FindMaxPriority();
void DeletePCB(int id);
void Run();
void	PrintPCB();
void Display_TerminateQueue();
void Display_BlockQueueForIO();
void Display_RunningQueue();
void Display_Ready();
void InsertHead(PCB* p);

void InitPCB() {
	srand((unsigned)time(NULL));
	int processNumber=rand()%10+1;//随机进程个数范围：1-10
	PCB* p=NULL;
	PCB* q=NULL;
	for (int i = 0; i < processNumber; i++) {
		q=new PCB;
		q->id = i+1;
		q->priority = rand() % 30 + 1;//优先级范围，为了显示效果更好，使优先级范围小一点
		q->state = Ready;
		q->timeslice = rand() % 10+1;//同理
		if (i == 0) {
			head = q;
			p= q;
		}
		else {
			p->next=q;
			p = p->next;
		}
	}
}
PCB* FindMaxPriority() {
	PCB* p = head;
	PCB* q =NULL;
	int max_priority = 0;
	if (head == NULL)return NULL;
	while (p) {
		if (p->priority > max_priority) {
			q = p;
			max_priority = p->priority;
		}
		p = p->next;
	}
	return q;
}
void DeletePCB( int id) {
	PCB* p=head;
	PCB* q=p->next;
	if (p->id == id) {
		p->next = NULL;
		head = q;
	}
	else {
		while (q->id!=id) {
			p = p->next;
			q = p->next;
		}
		p->next = q->next;
		q->next = NULL;
	}
}

void Run() {
	srand((unsigned)time(NULL));
	PCB* p=NULL;
	PCB* q= FindMaxPriority();
	/*假设：当上一次运行完成时时，cpu才会有空去（找可运行性的就绪状态进程）
		或者（检查唤醒条件是否满足也可以说去检查是否有唤醒信号，所以应放在此位置）
		注意：之所以先去检查唤醒而不是去找可运行性的就绪状态进程（即以下代码的顺序问题），
		是为了避免当所有可执行进程执行完了才去唤醒操作，不符合现实
		*/
	if (!blockQueueForIO.empty()) {
		//理论上我认为，因为在多次频繁检查IO占用情况时，IO一段时间都是被占用的，被唤醒的概率会比阻塞概率小
		int awakenedProbability = rand() % 8 + 1;//唤醒概率：1/8
		if (awakenedProbability == 4) {
			p = blockQueueForIO.front();
			blockQueueForIO.pop();
			p->state = Ready;
			InsertHead(p);
		}
	}
	if (runningQueue.empty()) {
		if (head!=NULL) {
			q->state = Running;
			DeletePCB(q->id);
			runningQueue.push(q);
		}else {
			//当运行队列、就绪队列为空时，此时资源一定不会占用，这也是唤醒概率的弊端,直接进入运行队列
			if (!blockQueueForIO.empty()) {
				p = blockQueueForIO.front();
				blockQueueForIO.pop();
				p->state = Running;
				runningQueue.push(p);
				//此时阻塞也不可能发生
				blockedProbability = 0;
			}
			else {
				return;
			}
		}
	}else {
		/*运行队列里的进程一定是可以执行的，而且在函数开头，此进程一定打印过，
		不会出现刚调入到运行队列还没有打印就被放入阻塞队列的的情况，
		由于当运行队列里的进程下个时间段可以执行时，再考虑它会不会阻塞
		*/
		blockedProbability = rand() % 4 + 1;//被阻塞概率:1/4
		//如果运行队列为空，即使达到阻塞概率也不可能会阻塞
		if (blockedProbability == 2&&head!=NULL) {
			p=runningQueue.front();
			runningQueue.pop();
			p->state = BlockForIO;
			blockQueueForIO.push(p);
			q->state = Running;
			DeletePCB(q->id);
			runningQueue.push(q);
		}
		p=runningQueue.front();
		p->timeslice -= 1;
		p->priority -= 1;
		if (p->timeslice < 1) {
			runningQueue.pop();
			p->state = Terminate;
			terminateQueue.push(p);
			if (q) {
				DeletePCB(q->id);
				q->state = Running;
				runningQueue.push(q);
			}
		}
		else if (q) {//若就绪队列没进程，一定不会阻塞并且当前进程也不会进入就绪队列，所以当就绪队列有进程是时，凭概率设置阻塞
			//当优先级不是最高时，当前进程一定会下台，所以下次也一定没有它运行的机会，阻塞的意义：当下次还有运行的机会时，给他阻塞，剥夺他运行的机会
			if (p->priority < q->priority) {
				runningQueue.pop();
				p->state = Ready;
				InsertHead(p);
				q->state = Running;
				DeletePCB(q->id);
				runningQueue.push(q);
			}
		}
	}
	PrintPCB();
	cout << endl << endl<<endl<<endl;
	Run();
}
void InsertHead(PCB* p) {
	p->next = head->next;
	head->next = p;
}
void Display_Ready() {
	PCB* p = head;
	while (p) {
		cout << "ID:	" << p->id << "		Priority:	" << p->priority << "		Timeslice:		" << p->timeslice << endl;
		p = p->next;
	}
}
void Display_RunningQueue() {
	if (!runningQueue.empty()) {
		PCB* t = runningQueue.front();
		cout << "ID:	" << t->id << "		Priority:	" << t->priority << "		Timeslice:		" << t->timeslice << endl;
	}
}
void Display_BlockQueueForIO() {
	PCB* tempPCBArray[10];
	int n = 0;
	PCB* t;
	if (!blockQueueForIO.empty()) {
		while (!blockQueueForIO.empty()) {
			t = blockQueueForIO.front();
			tempPCBArray[n] = t;
			n++;
			cout << "ID:	" << t->id << "		Priority:	" << t->priority << "		Timeslice:		" << t->timeslice << endl;
			blockQueueForIO.pop();
		}
		for (int i = 0; i < n; i++) {
			blockQueueForIO.push(tempPCBArray[i]);
		}
	}
}
void Display_TerminateQueue() {
	PCB* tempPCBArray[10];
	int n = 0;
	PCB* t;
	if (!terminateQueue.empty()) {
		while (!terminateQueue.empty()) {
			t = terminateQueue.front();
			tempPCBArray[n] = t;
			n++;
			cout << "ID:	" << t->id << "		Priority:	" << t->priority << "		Timeslice:		" << t->timeslice << endl;
			terminateQueue.pop();
		}
		for (int i = 0; i < n; i++) {
			terminateQueue.push(tempPCBArray[i]);
		}
	}
}
void	PrintPCB() {
	cout << "就绪队列：" << endl; Display_Ready(); cout << endl;
	cout << "运行队列："<<endl; Display_RunningQueue(); cout << endl;
	cout << "IO阻塞队列：" << endl; Display_BlockQueueForIO(); cout << endl;
	cout << "终止队列：" << endl; Display_TerminateQueue(); cout << endl;
}

int main() {
	InitPCB();
	Run();
	return 0;
}

