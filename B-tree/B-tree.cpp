// C++ 实现 B 树的插入
#include<iostream>
using namespace std;

// B 树节点类
class BTreeNode
{
    int *keys;       // 键值数组
    int t;           // 最小度数（决定键数量的范围）
    BTreeNode **C;   // 子节点指针数组
    int n;           // 当前键的数量
    bool leaf;       // 是否是叶子节点
public:
    BTreeNode(int _t, bool _leaf);           // 构造函数

    void insertNonFull(int k);               // 用于在非满节点插入新键的辅助函数 调用本函数时要求节点未满

    void splitChild(int i, BTreeNode *y);    // 分裂本节点的第 i 个子节点 y  注意：调用时子节点 y 必须已满

    void traverse();                         // 遍历以本节点为根的子树

    BTreeNode *search(int k);                // 在子树中搜索键 k  未找到时返回 NULL  

    BTreeNode *Lastsearch(int k,BTreeNode *a);   // 在子树中搜索键 k 的父节点 未找到时返回 NULL  

    friend class BTree;                      // 声明 BTree 为友元类以便访问私有成员
   
};

// B 树类
class BTree
{
    BTreeNode *root; // 根节点指针
    int t;  // 最小度数
public:
    // 构造函数（初始化空树）
    BTree(int _t)
    {  root = NULL;  t = _t; }

    // 遍历整棵树
    void traverse()
    {  if (root != NULL) root->traverse(); }

    // 在树中搜索键
    BTreeNode* search(int k)
    {  return (root == NULL)? NULL : root->search(k); }

    // 在树中搜索键的父节点
    BTreeNode* Lastsearch(int k)
    {  return (root == NULL)? NULL : root->Lastsearch(k,root); }

    void btree_delete_key(int k);//键值的删除

    // 插入新键的主函数
    void insert(int k);

    //借位
    void btree_jiewei(BTreeNode *node,BTreeNode *f,int index,int isleaf);

    //合并
    void btree_hebin(BTreeNode *node,BTreeNode *f,int index);

};

// BTreeNode 的构造函数
BTreeNode::BTreeNode(int t1, bool leaf1)
{
    // 复制最小度数和叶子属性
    t = t1;
    leaf = leaf1;

    // 为键和子节点分配最大可能空间
    keys = new int[2*t-1];
    C = new BTreeNode *[2*t];

    // 初始化键数量为0
    n = 0;
}

// 遍历以本节点为根的子树
void BTreeNode::traverse()
{
    // 共有 n 个键和 n+1 个子节点
    int i;
    for (i = 0; i < n; i++)
    {
        // 若非叶子节点，先遍历子节点 C[i]
        if (leaf == false)
            C[i]->traverse();
        cout << " " << keys[i];
    }

    // 最后遍历剩余的子节点
    if (leaf == false)
        C[i]->traverse();
}

// 在子树中搜索键 k
BTreeNode *BTreeNode::search(int k)
{
    // 找到第一个大于等于 k 的键
    int i = 0;
    while (i < n && k > keys[i])
        i++;

    // 找到则返回当前节点
    if (keys[i] == k)
        return this;

    // 未找到且为叶子节点时返回空
    if (leaf == true)
        return NULL;

    // 递归搜索对应的子节点
    return C[i]->search(k);
}

// 在子树中搜索键 k 的父节点
BTreeNode *BTreeNode::Lastsearch(int k,BTreeNode *a)
{

    // 找到第一个大于等于 k 的键
    int i = 0;
    while (i < n && k > keys[i])
        i++;

    // 找到则返回当前节点
    if (keys[i] == k)
        return a;

    // 未找到且为叶子节点时返回空
    if (leaf == true)
        return NULL;

    // 递归搜索对应的子节点
    return C[i]->Lastsearch(k,this);
}

// B 树插入键的主函数
void BTree::insert(int k)
{
    // 空树情况处理
    if (root == NULL)
    {
        root = new BTreeNode(t, true);
        root->keys[0] = k;  // 插入键
        root->n = 1;        // 更新键数量
    }
    else // 非空树处理
    {
        // 根节点已满时树长高
        if (root->n == 2*t-1)
        {
            BTreeNode *s = new BTreeNode(t, false); // 创建新根
            s->C[0] = root;           // 旧根成为子节点
            s->splitChild(0, root);   // 分裂旧根

            // 确定新键插入位置
            int i = 0;
            if (s->keys[0] < k)
                i++;
            s->C[i]->insertNonFull(k);

            root = s; // 更新根节点
        }
        else
            root->insertNonFull(k); // 直接插入非满根
    }
}

// 在非满节点插入键的实现
void BTreeNode::insertNonFull(int k)
{
    int i = n-1; // 从最右端开始

    // 叶子节点插入
    if (leaf == true)
    {
        // 寻找插入位置并后移较大键
        while (i >= 0 && keys[i] > k)
        {
            keys[i+1] = keys[i];
            i--;
        }
        keys[i+1] = k; // 插入新键
        n++;            // 更新计数
    }
    else // 内部节点处理
    {
        // 寻找合适的子节点
        while (i >= 0 && keys[i] > k)
            i--;

        // 检查子节点是否已满
        if (C[i+1]->n == 2*t-1)
        {
            splitChild(i+1, C[i+1]); // 分裂子节点

            // 分裂后决定插入方向
            if (keys[i+1] < k)
                i++;
        }
        C[i+1]->insertNonFull(k); // 递归插入
    }
}

// 分裂子节点的实现
void BTreeNode::splitChild(int i, BTreeNode *y)
{
    // 创建新节点存储后 t-1 个键
    BTreeNode *z = new BTreeNode(y->t, y->leaf);
    z->n = t - 1;

    // 复制键和子节点
    for (int j = 0; j < t-1; j++)
        z->keys[j] = y->keys[j+t];

    if (!y->leaf)
        for (int j = 0; j < t; j++)
            z->C[j] = y->C[j+t];

    y->n = t - 1; // 更新原节点键数

    // 为新子节点腾出空间
    for (int j = n; j >= i+1; j--)
        C[j+1] = C[j];

    C[i+1] = z; // 链接新节点

    // 移动键并插入中间键
    for (int j = n-1; j >= i; j--)
        keys[j+1] = keys[j];

    keys[i] = y->keys[t-1]; // 提升中间键
    n++; // 更新当前节点键数
}

//下溢后借位
void BTree::btree_jiewei(BTreeNode *node,BTreeNode *f,int index,int isleaf){

    //如果左兄弟够找左兄弟借
    if(index-1>=0&&f->C[index-1]->n-1>=t/2){
        int fk=f->keys[index-1];    //获取父节点的键值
        BTreeNode *bs;
        if(!isleaf){
        //如果左兄弟键值的右孩子存在，则记录左兄弟键值的右孩子
        if(f->C[index-1]->C[f->C[index-1]->n]->n>0){
            bs = f->C[index-1]->C[f->C[index-1]->n];
            //清空子树
            for(int j=0;j<f->C[index-1]->C[f->C[index-1]->n]->n;++j){
                f->C[index+1]->C[f->C[index-1]->n]->keys[j]==0;
            }
            f->C[index+1]->C[f->C[index-1]->n]=nullptr;
            }
        }

        for(int j=node->n-1;j>=0;--j){
            node->keys[j+1]=node->keys[j];
        }
        node->n++;                //将叶子节点从删除键值往后推
        node->keys[0]=fk;         //将父节点的键值给叶子节点

        //将左兄弟的键值放到父亲节点
        int sk=f->C[index-1]->keys[f->C[index-1]->n-1];
        f->C[index-1]->keys[f->C[index-1]->n-1]=0;
        f->C[index-1]->n--;
        f->keys[index-1]=sk;

        if(!isleaf){
            //如果父节点从兄弟借的键值有孩子也要一起带过来
            f->C[index]->C[0]=bs;
        }
    }
    //如果右兄弟够找右兄弟借
    else if(f->C[index+1]->n-1>=t/2){
        int fk=f->keys[index+1];   //获取父节点的键值
        BTreeNode *bs;
        if(!isleaf){    
        //如果右兄弟键值的左孩子存在，则记录右兄弟键值的左孩子
        if(f->C[index+1]->C[0]->n>0){
            bs = f->C[index+1]->C[0];
            //清空子树
            for(int j=0;j<f->C[index+1]->C[0]->n;++j){
                f->C[index+1]->C[0]->keys[j]==0;
            }
            f->C[index+1]->C[0]=nullptr;
            }
        }

        node->keys[node->n]=fk;      //父节点的键值添加到尾     
        node->n++;                    

        //将右兄弟的键值放到父亲节点
        int sk=f->C[index+1]->keys[0];
        for(int j=0;j<f->C[index+1]->n;++j){
            f->C[index+1]->keys[j]=f->C[index+1]->keys[j+1];    
        }
        f->keys[index+1]=sk;
        f->C[index+1]->n--;

        if(!isleaf){
        //如果父节点从兄弟借的键值有孩子也要一起带过来
        f->C[index]->C[f->C[index]->n]=bs;
        }
    }
    //如果都不够就合并
    else{
        btree_hebin(node,f,index);
    }
}

//无法借位就合并
void BTree::btree_hebin(BTreeNode *node,BTreeNode *f, int index){

    //如果左兄弟存在
    if(f->t>1&&index>0){
        int fk=f->keys[index-1];  //获取要合并的父键值
        BTreeNode *gf=Lastsearch(fk);   //祖父节点

        for(int j=0;j<=f->C[index]->n-1;++j){
            f->C[index]->keys[j+1]=f->C[index]->keys[j];
        }
        f->C[index]->keys[0]=fk;
        f->C[index]->n++;                         //合并父键值


        for(int z=node->n-1;z>=0;--z)  {
            for(int j=0;j<=f->C[index]->n-1;++j){
            f->C[index]->keys[j+1]=f->C[index]->keys[j];
            }
            f->C[index]->keys[0]=f->C[index-1]->keys[z];
            f->C[index]->n++;  
            f->C[index-1]->n--;      
        }                                           //合并叶子键值      


        //收尾
        f->C[index-1]=nullptr;
        for(int j=index;j<f->n;++j){
            f->keys[j]=f->keys[j+1];
        }
        // 调整父节点的子节点指针数组
        for (int j = index; j <= f->n; j++) { // 注意子节点数目为n+1
            f->C[j] = f->C[j+1];
        }        
        f->keys[f->n]=0;
        f->n--;

         //检查合并后父节点是否下溢
        if(f->n-1<t/2){
            //如果下溢找父节点兄弟借

            //找k的祖父节点的键值的位置
            int gindex=0;
            while(index< gf->n&& fk > gf ->keys[gindex]){
                gindex++;
            }
            btree_jiewei(f,gf,gindex,0);
        }    

    }
    //如果右兄弟存在
    else if(f->t>1&&index<f->n-1){
        int fk=f->keys[index+1];  //获取要合并的父键值
        BTreeNode *gf=Lastsearch(fk);   //祖父节点

         //合并父键值
        f->C[index]->keys[f->C[index]->n]=fk;
        f->C[index]->n++;                        

        //合并叶子键值
        for(int j=0;j<f->C[index+1]->n;++j){
            f->C[index]->keys[f->C[index]->n]=f->C[index+1]->keys[j];
            f->C[index+1]->keys[j]=0;
            f->C[index]->n++;                            
        }


        //收尾
        f->C[index+1]=nullptr;
        for(int j=index;j<f->n;++j){
            f->keys[j]=f->keys[j+1];
        }
        // 调整父节点的子节点指针数组
        for (int j = index; j <= f->n; j++) { // 注意子节点数目为n+1
        f->C[j] = f->C[j+1];
        }
        f->keys[f->n-1]=0;
        f->n--;
         //检查合并后父节点是否下溢
        if(f->n-1<t/2){
            //如果下溢找父节点兄弟借

            //找k的祖父节点
            int gindex=0;
            while(index< gf->n&& fk > gf ->keys[gindex]){
                gindex++;
            }
            btree_jiewei(f,gf,gindex,0);
        }    

    }
}


//键值的删除
void BTree::btree_delete_key(int k){

    //找k节点
    int idx=0;
    BTreeNode *node=search(k);
    while(idx< node->n&& k > node ->keys[idx]){
        idx++;
    }

   //找k的父节点
    int index=0;
    BTreeNode *f=Lastsearch(k);
    while(index< f->n&& k > f ->keys[index]){
        index++;
    }                                                

    //如果要删除根节点中的键值
    if(node==root){
        //根节点只有一个值
        if(node->n==1){
            //如果根节点有孩子
            if(node->t>0){
                while(1){
                    if(!node->leaf){
                        node=node->C[node->t-1];//找拥有叶子的节点
                    }
                    else{
                        root->keys[0]=node->keys[node->n-1];//更新根节点和前驱
                        node->keys[node->n-1]=0;
                        node->n--;
                        break;
                    }
                }
            }
            //根节点没有孩子
            else{
                node->keys[0]=0;
                node->n--;
                if(node->n==0){
                    root=nullptr;
                }
            }
        }
        //根节点有多个值
        else {
            for(int i=idx;i<node->n;++i){
                node->keys[i]=node->keys[i+1];
            }
            node->n--;
        }
    }
    //删除节点键值
    else{
        //是否是叶子节点
        if(node->leaf){
            for(int i=idx;i<node->n;++i){
                node->keys[i]=node->keys[i+1];
            }
            node->n--;
            //删除之后出现下溢进行借位
            if(node->n<t/2){
            btree_jiewei(node,f,index,1);
            }
        }
        //不是叶子节点
        else{
            //检查删除后是否下溢
            if(node->n<t/2){
                //下溢找前后驱,先检查前后驱是否存在
                if(node->C[idx+1]->n>0){
                    int latekeys=node->C[idx+1]->keys[0]; //后驱键值
                    node->keys[idx]=latekeys;             //消除下溢同时删除k键值
                    //收尾处理孩子,检查后驱孩子是否出现新的下溢出
                    for(int j=0;j<node->C[idx+1]->n;++j){
                        node->C[idx+1]->keys[j]=node->C[idx+1]->keys[j+1];
                    }
                    node->C[idx+1]->keys[node->C[idx+1]->n-1]=0;
                    node->C[idx+1]->n--;
                    if(node->C[idx+1]->n<t/2){
                        //孩子出现新的下溢,先借，借不到就合并
                        btree_jiewei(node->C[idx+1],node,idx+1,0);
                    }
                }
                //检查前驱
                else if(node->C[idx]->n>0){
                    int headkeys=node->C[idx]->keys[node->C[idx]->n-1]; //前驱键值
                    node->keys[idx]=headkeys;             //消除下溢同时删除k键值
                    //收尾处理孩子,检查后驱孩子是否出现新的下溢出
                    node->C[idx]->keys[node->C[idx]->n-1]=0;
                    node->C[idx]->n--;
                    if(node->C[idx]->n<t/2){
                        //孩子出现新的下溢,先借，借不到就合并
                        btree_jiewei(node->C[idx],node,idx+1,0);
                    }
                }
                //没有前后驱孩子直接删
                else{
                    for(int i=idx;i<node->n;++i){
                        node->keys[i]=node->keys[i+1];
                    }
                    node->keys[node->n-1]=0;
                    node->n--;
                }
            }
        }
    }
}

// 主函数用于测试上述功能
int main()
{
    BTree t(5); // 创建最小度数为3的B树
    t.insert(10);
    t.insert(20);
    t.insert(30);    t.insert(92);
    t.insert(93);
    t.insert(40);
    t.insert(41);
    t.insert(42);
    t.insert(43);
    t.insert(44);
    t.insert(45);
    t.insert(46);
    t.insert(47);
    t.insert(50);
    t.insert(51);
    t.insert(53);
    t.insert(57);
    t.insert(60);
    t.insert(65);
    t.insert(68);
    t.insert(72);
    t.insert(74);
    t.insert(76);
    t.insert(83);
    t.insert(86);
    t.insert(90);
 
    cout << "Traversal of the constucted tree is "; // 输出构造后的树的中序遍历
    while(1){
    t.traverse(); // 执行遍历操作
    cout<<endl;
    int k;
    cin>>k;
    t.btree_delete_key(k);
    }
    // int k = 6;
    // // 查找键值6并输出结果
    // (t.search(k) != NULL)? cout << "\nPresent" : cout << "\nNot Present";
 
    // k = 15;
    // // 查找键值15并输出结果
    // (t.search(k) != NULL)? cout << "\nPresent" : cout << "\nNot Present";
    return 0;
}
