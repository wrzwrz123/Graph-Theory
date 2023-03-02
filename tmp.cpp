/*
* ���ߣ�
���ܣ�ģ�����״̬ת������������Χ��1-10��
4��PCB״̬��ʶ������1.����Ready��2.����Running��3.����Block��4.��ֹTerminate
��Run��������ȥģ���CPU��run������Run����Ӧ������������Ĵ�������4�ֶ��ж�Ӧ�Ĵ����������
		��1���������У�Ϊ�˷�����������ȼ�����Ȼ��priority_queue�����㣬���Ҳ��ø���Ϥ������ṹ����
			����_1->����_1
			����_1->����_2

		��2�����ж���
			����_2->����_1
			����_2->����_2
			����_2->����_3
			����_2->��ֹ_4
		
		��3���������У����Ѹ��ʣ�1/8������������:1/4��
			����_3->����->����_1
			����_3->����->����_1���������������ж���Ϊ��ʱ��������������������ֱ�ӻ��ѵ����У�
			����_3->����_3//���Ѹ���û�������ʱ��

		��4����ֹ���У�
			����ֹ������Ľ��̲���Ҫ���д���
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
	int priority = 0;//��ʼ������ʱ���������,��ռʽ���ȼ�����
	int timeslice = 0;//��ʼ������ʱ���������
	State state = Ready;//��������������CPU��һ����Դ����ʼ״̬Ϊ����
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
	int processNumber=rand()%10+1;//������̸�����Χ��1-10
	PCB* p=NULL;
	PCB* q=NULL;
	for (int i = 0; i < processNumber; i++) {
		q=new PCB;
		q->id = i+1;
		q->priority = rand() % 30 + 1;//���ȼ���Χ��Ϊ����ʾЧ�����ã�ʹ���ȼ���ΧСһ��
		q->state = Ready;
		q->timeslice = rand() % 10+1;//ͬ��
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
	/*���裺����һ���������ʱʱ��cpu�Ż��п�ȥ���ҿ������Եľ���״̬���̣�
		���ߣ���黽�������Ƿ�����Ҳ����˵ȥ����Ƿ��л����źţ�����Ӧ���ڴ�λ�ã�
		ע�⣺֮������ȥ��黽�Ѷ�����ȥ�ҿ������Եľ���״̬���̣������´����˳�����⣩��
		��Ϊ�˱��⵱���п�ִ�н���ִ�����˲�ȥ���Ѳ�������������ʵ
		*/
	if (!blockQueueForIO.empty()) {
		//����������Ϊ����Ϊ�ڶ��Ƶ�����IOռ�����ʱ��IOһ��ʱ�䶼�Ǳ�ռ�õģ������ѵĸ��ʻ����������С
		int awakenedProbability = rand() % 8 + 1;//���Ѹ��ʣ�1/8
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
			//�����ж��С���������Ϊ��ʱ����ʱ��Դһ������ռ�ã���Ҳ�ǻ��Ѹ��ʵı׶�,ֱ�ӽ������ж���
			if (!blockQueueForIO.empty()) {
				p = blockQueueForIO.front();
				blockQueueForIO.pop();
				p->state = Running;
				runningQueue.push(p);
				//��ʱ����Ҳ�����ܷ���
				blockedProbability = 0;
			}
			else {
				return;
			}
		}
	}else {
		/*���ж�����Ľ���һ���ǿ���ִ�еģ������ں�����ͷ���˽���һ����ӡ����
		������ָյ��뵽���ж��л�û�д�ӡ�ͱ������������еĵ������
		���ڵ����ж�����Ľ����¸�ʱ��ο���ִ��ʱ���ٿ������᲻������
		*/
		blockedProbability = rand() % 4 + 1;//����������:1/4
		//������ж���Ϊ�գ���ʹ�ﵽ��������Ҳ�����ܻ�����
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
		else if (q) {//����������û���̣�һ�������������ҵ�ǰ����Ҳ�������������У����Ե����������н�����ʱ��ƾ������������
			//�����ȼ��������ʱ����ǰ����һ������̨�������´�Ҳһ��û�������еĻ��ᣬ���������壺���´λ������еĻ���ʱ���������������������еĻ���
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
	cout << "�������У�" << endl; Display_Ready(); cout << endl;
	cout << "���ж��У�"<<endl; Display_RunningQueue(); cout << endl;
	cout << "IO�������У�" << endl; Display_BlockQueueForIO(); cout << endl;
	cout << "��ֹ���У�" << endl; Display_TerminateQueue(); cout << endl;
}

int main() {
	InitPCB();
	Run();
	return 0;
}

