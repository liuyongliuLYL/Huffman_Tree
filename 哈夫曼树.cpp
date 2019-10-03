#include <iostream>
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <map>
using namespace std;
#define MAX 100
#define INF 0x3f3f3f3f3f3f3f3f

struct Node {//树的节点
	float weight;//权值
	char ch;//字符，非叶子节点值为'\0'
	Node* lchild;
	Node* rchild;
	Node* parent;
};

struct Code {
	char ch;
	vector<int>bin;//储存字符的二进制编码
};

vector<Node*> forest,forest1;//森林,存放树的头指针,1为备份的原始森林

void Select(Node** s1, Node** s2)
{
	int a = 0, b = 0;//记录最小的两个权值的位置
	for (int i = 0; i < forest.size(); i++) 
		if (forest[i]->weight < forest[a]->weight) a = i;
	if (a == 0) b = 1;//防止ab相同
	for (int i = 0; i < forest.size(); i++)
	{
		if (i == a) continue;
		if (forest[i]->weight < forest[b]->weight) b = i;
	}
	//cout << forest[a]->weight << " " << forest[b]->weight << endl;
	*s1 = forest[a];
	*s2 = forest[b];
	return;
}

Node* HuffmanCreat(string s, float *w,int n)
{//构造赫夫曼树
	//1 创建初始树，存入vector
	forest.clear();
	for (int i = 1; i <= n; i++)
	{
		Node* tree = new(Node);
		tree->weight = w[i];
		tree->ch = s[i - 1];
		tree->lchild = NULL;
		tree->rchild = NULL;
		tree->parent = NULL;
		forest.push_back(tree);
	}
	//2 备份原始森林，便于之后编码
	forest1.clear();
	for (int i = 0; i < forest.size(); i++) forest1.push_back(forest[i]);
	//3 构造树
	while (forest.size() != 1)//直到森林中只有一棵树
	{
		//cout << forest.size() << endl;
		Node* s1=NULL, *s2=NULL, *New_Tree;
		Select(&s1, &s2);//从森林中选出两颗权值最小的树，记为s1,s2

		//确立这三个节点的关系，合成一颗新树New_Tree
		New_Tree = new(Node);
		New_Tree->ch = '\0';
		New_Tree->weight = s1->weight + s2->weight;
		New_Tree->lchild = s1;
		New_Tree->rchild = s2;
		New_Tree->parent = NULL;
		s1->parent = New_Tree;
		s2->parent = New_Tree;
		//从森林中删除旧树，加入新树
		forest.push_back(New_Tree);
		for (int i = 0; i < forest.size(); i++)
		{
			if (forest[i] == s1 || forest[i] == s2) forest.erase(forest.begin() + i), i--;
		}
	}
	return forest[0];//返回森林中的唯一一棵树，即赫夫曼树
}

void HuffmanCoding(Node* HFm,Code* HC,int n,int* deep)//生成赫夫曼编码规则HC[]
{//forest1森林中的指针指向赫夫曼树的叶子节点
	int de;
	for (int i = 0; i < forest1.size(); i++)
	{//用栈储存 
		stack<bool>st;
		de = 1;//每个叶子节点的深度
		Node* p = forest1[i];
		//HC[i] = new(Code);
		HC[i].ch = p->ch;
		//从叶子节点到根结点
		while (p->parent)//非根结点
		{//左0右1
			if (p->parent->lchild == p) st.push(0);
			else st.push(1);
			p = p->parent;
			de++;
		}
		while (!st.empty())//将栈中的二进制编码存入HC[i]
		{
			HC[i].bin.push_back(st.top());
			st.pop();
		}
		*deep = max(*deep, de);//最大值即树的深度
	}
	FILE * fid = fopen("Coding rule.txt", "w");
	for (int i = 0; i < n; i++)
	{
		fprintf(fid, "%c\t", HC[i].ch); cout << HC[i].ch << "\t";
		for (int j = 0; j < HC[i].bin.size(); j++) fprintf(fid, "%d", HC[i].bin[j]), cout << HC[i].bin[j];
		fprintf(fid, "\n");
		cout << endl;
	}
	fclose(fid);
	cout << "哈夫曼编码规则已写入Coding rule！" << endl;
}

void CheckCoding(Code* HC,int n)//对电文编码
{
	vector<char> a;
	//文件化
	FILE * p = fopen("ToBeTra.txt", "r");
	if(p==NULL) {
		cout<<"打开文件失败，请在目录下创建ToBeTra.txt，并写入需要编码的文件"<<endl;
		return ;
	}
	while (!feof(p)) a.push_back(fgetc(p));
	//cout << "请输入需要编码的电文：";
	//getchar();
	//getline(cin, a);
	vector<int>bin;//用于存储电文二进制编码
	for (int i = 0; i < a.size(); i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (HC[j].ch == a[i]) {
				for (int k = 0; k < HC[j].bin.size(); k++) bin.push_back(HC[j].bin[k]);//将此字符的编码加入bin中
				break;
			}
		}
	}
	cout << "电文编码为：";
	for (int i = 0; i < bin.size(); i++) 
	{
		if(i%40==0) cout << endl;
		cout << bin[i];
	}
	cout << endl;
	FILE * fid = fopen("CodeFil.txt", "w");
	for (int i = 0; i < bin.size(); i++)
		fprintf(fid, "%d", bin[i]);
	cout << "电文编码已写入CodeFil.txt！" << endl;
	fclose(fid);
}

void HuffmanTranslate(Code* HC,int n)//哈夫曼译码
{
	vector<int>bin;//存储电文二进制编码
	bin.clear();
	/*cout << "输入二进制代码：" << endl;
	string t;
	cin >> t;
	for (int i = 0; i < t.size(); i++) {
		if(t[i]=='0') bin.push_back(0);//输入string,再导入bin中
		else bin.push_back(1);
	}*/
	//文件化
	FILE * p = fopen("CodeFile.txt", "r");
	if(p==NULL) {
		cout<<"打开文件失败，请在目录下创建CodeFile.txt，并写入需要译码的文件"<<endl;
		return ;
	}
	while (!feof(p)) {
		char y = fgetc(p);
		if(y=='0') bin.push_back(0);
		else bin.push_back(1);
	}
	//bin中多读了一位结束符'\0'
	//for (int i = 0; i < bin.size(); i++) cout << bin[i];

	vector<int>T;//临时
	vector<char> C;//储存译码结果
	for (int i = 0; i < bin.size()-1; i++)
	{
		T.push_back(bin[i]);
		//判断T与HC中是否有匹配
		for (int j = 0; j < n; j++){
			if (HC[j].bin == T) {
				C.push_back(HC[j].ch);
				T.clear();
				break;
			}
		}
	}
	//cout << T.size() << endl;
	if (!T.empty()) cout << "ERROR" << endl;
	else {
		FILE * fid1 = fopen("Textfile.txt", "w");
		for (int i = 0; i < C.size(); i++) cout << C[i], fprintf(fid1, "%c", C[i]);
		cout << endl;
		cout << "译文已写入Textfile.txt！" << endl;
		fclose(fid1);
	}
}

void Treeprinting(Node* HFm,int deep)//印哈夫曼树
{//先层序遍历,每多一层，宽变两倍,打印树的总宽度为 2^deep
	FILE * fid3 = fopen("TreePrint.txt", "w");//文件化
	queue<Node*>Q;
	Q.push(HFm);
	int m = (1 << deep);//总宽度
	for (int i = 1; i <= m; i++) cout << "*", fprintf(fid3, "*"); 
	cout << endl; fprintf(fid3, "\n");
	for (int h = 1; h <= deep; h++)//h为当前深度
	{
		for (int i = 1; i <= (1 << (h - 1)); i++)
		{
			Node* p=Q.front();
			Q.pop();
			if (p) {
				if (p->ch != '\0') cout <<"("<< p->ch<<","<<p->weight<<")", fprintf(fid3, "%c", p->ch);
				else cout<<"[" << p->lchild->weight<<"+"<<p->rchild->weight<<"]", fprintf(fid3, "0");
			}
			else cout << " ", fprintf(fid3, " ");
			//第h行间距为2^(deep-h+1)-1
			for (int j = 1; j < (1 << (deep + 1 - h)); j++) cout << " ", fprintf(fid3, " ");
			//加入下一行的节点
			if (p){
				Q.push(p->lchild);
				Q.push(p->rchild);
			}
			else {
				Q.push(NULL);
				Q.push(NULL);
			}
		}
		cout << endl; fprintf(fid3, "\n");
	}
	for (int i = 1; i <= m; i++) cout << "*", fprintf(fid3, "*");
	cout << endl; fprintf(fid3, "\n");
	fclose(fid3);
	cout << "字符形式的哈夫曼树已写入文件TreePrint.txt！" << endl;
}

void Delete(Node* p)//释放哈夫曼树
{
	if (p) {
		Delete(p->lchild);
		Delete(p->rchild);
		delete(p);
	}
}

int main()
{
	printf("---------------------------------------------\n");
	printf("       赫夫曼编码和译码        \n");
	printf("        1.初始化               \n");
	printf("        2.输入对应的正文内容   \n");
	printf("        3.进行赫夫曼编码       \n");
	printf("        4.进行赫夫曼译码       \n");
	printf("        5.印哈夫曼树           \n");
	printf("        6.退出赫夫曼操作       \n");
	printf(" ---------------------------------------------\n");
	int deep = 0;//哈夫曼树的深度
	int n;//字符数
	float w[MAX];//权值
	string s;//字符串
	Node* HFm=NULL;
	Code* HC=NULL;
	int opt;
	int f=0;
	while (1)
	{
		cout<<"请输入对应操作的序号："; 
		cin >> opt;
		switch (opt)
		{
		case 1://初始化 
			if(HFm) Delete(HFm);
			if(HC) delete[] HC;
			HFm = NULL;
			HC = NULL;
			cout<<"初始化成功"<<endl; 
			break;
		case 2://输入对应的正文内容
			if(HFm && HC){
				cout<<"请先初始化"<<endl;
				break; 
			}
			while (1)
			{ 
				cout << "请输入字符个数：" << endl;
				cin>>n;
				if (n > 0) break;
				else cout << "ERROR" << endl;
			}
			getchar();
			while (1)
			{
				map<char,int>mp;
				cout << "请输入字符：" << endl;
				
				getline(cin, s);
				if(s.length()!=n) { cout << "字符个数错误" << endl;continue;}
				//判重
				int f=0;
				for(int i=0;i<s.length();i++)
				{
					if(mp[s[i]]==0) mp[s[i]]=1;
					else{
						cout<<"字符重复，请重新读入。"<<endl;
						f=1;
						break; 
					}
				} 
				if(f==0) break;//读入成功 
			}
			cout << "请输入对应权值(权值间空格)：" << endl;
			for (int i = 1; i <= n; i++) cin >> w[i];
			HFm = HuffmanCreat(s, w, n);//构建赫夫曼树
			HC = new Code[n + 2];//生成赫夫曼编码规则
			HuffmanCoding(HFm, HC, n, &deep);
			cout<<"现在可以进行编码和译码"<<endl; 
			break;
		case 3://3.进行赫夫曼编码 
			if(HFm==NULL || HC==NULL){
				cout<<"请先输入正文内容，构建哈夫曼树以及编码规则"<<endl;
				break; 
			}
			CheckCoding(HC, n);
			break;
		case 4://进行赫夫曼译码 
			if(HFm==NULL || HC==NULL){
				cout<<"请先输入正文内容，构建哈夫曼树以及编码规则"<<endl;
				break; 
			}
			HuffmanTranslate(HC, n);
			break;
		case 5://印哈夫曼树
			if(HFm==NULL || HC==NULL){
				cout<<"请先输入正文内容，构建哈夫曼树以及编码规则"<<endl;
				break; 
			}
			Treeprinting(HFm, deep);
			break;
		case 6://退出赫夫曼操作
			Delete(HFm);
			delete[] HC;
			HFm = NULL;
			HC = NULL;
			f=1;
			cout << "退出成功！" << endl;
			break;
		default:
			cout<<"ERROR!!!"<<endl;			
		}
		if(f) break;
	}
	system("PAUSE");
	return 0;
}
/*
5
abcde
1 2 7 8 13

8
abcdefgh
0.05 0.29 0.07 0.08 0.14 0.23 0.03 0.11

27
 ABCDEFGHIJKLMNOPQRSTUVWXYZ
186 64 13 22 32 103 21 15 47 57 1 5 32 20 57 63 15 1 48 51 80 23 8 18 1 16 1
THIS PROGRAME IS MY FAVORITE
*/
