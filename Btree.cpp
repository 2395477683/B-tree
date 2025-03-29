#include<iostream>

using namespace std;

#define M 6
#define DEGREE M/2

typedef int KEY_VALUE;


//数的节点
struct btree_node{
    KEY_VALUE *key; //节点中的数据
    btree_node **childerns;//节点的孩子

    int num;//数据的数量
    int leaf;//是否是叶子节点

};


//树
struct btree{
    btree_node *root;
};


//创建新的节点
btree_node *btree_create_node(btree *T){

}


//x节点的第i个孩子进行分裂
void btree_split_child(btree *T,btree_node *x,int i){
    
    btree_node *y=x->childerns[i];//要分裂的孩子i

    btree_node *z=malloc();//分裂的新节点

    for(int j=0;j<DEGREE-1;j++){
        z[j].key=y[j].key;
    }//要分裂的数据

    if(y->leaf==0){
        for(int j=0;j<DEGREE;++j){
            z[j].childerns=y[j].childerns;
        }
    }//转移孩子

    y->num=DEGREE-1;
}


//如果孩子不是满的插入
void btree_insert_nonfull(btree *T,btree_node *x,KEY_VALUE k){

    int i=x->num;

    if(x->leaf=1){

    }
    else{
        while(i>=0&&x->key[i]>k){
            i--;
        };
        if(x->childerns[i+1]->num==M-1){
            btree_split_child(T,x,i+1);
        }
        btree_insert_nonfull(T,x->childerns[i+1],k);
    }

}


//树的插入
void btree_insert(btree *T,KEY_VALUE key){

    //创建一个节点r并存储根节点
    btree_node *r=T->root;

    //如果孩子是满的
    if(r->num==M-1){

        //创建新的空节点node
        btree_node *node =btree_create_node(T);


        T->root=node;
        
        node->childerns[0]=r;

        btree_split_child(T,node,0);

    }
    //如果孩子不是满的
    else{
        btree_insert_nonfull(T,r,key);
    }


}


void btree_merge(btree *T,btree_node *x ,int idx){
    
}



//树的删除
void btree_delete_key(btree *T,btree_node *node,KEY_VALUE k){

    int idx=0;
    //找节点
    while(idx< node->num&& k > node ->key[idx]){
        idx++;
    }
    if(k==node->key[idx]){
        if(node->leaf){

        }
        else if(node->childerns[idx]->num>=DEGREE){

        }
        else if(node->childerns[idx+1]->num>=DEGREE){

        }
        else{

        }
    }
}



int main(){

}