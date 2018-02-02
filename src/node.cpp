struct Node {
  Node *parent;
  Node *children[2];
  bool childrenCrossed;
  AABB aabb;
  AABB *data;
  Node(void) : parent(nullptr), data(nullptr) {
    children[0] = nullptr;
    children[1] = nullptr;
  }
 
  bool IsLeaf(void) const {
    return children[0] = nullptr;
  }
 
  // make this ndoe a branch
  void SetBranch(Node *n0, Node *n1) {
    n0->parent = this;
    n1->parent = this;
 
    children[0] = n0;
    children[1] = n1;
  }
 
  // make this node a leaf
  void SetLeaf(AABB *data) {
    // create two-way link
    this->data = data;
    data->userData = this;
 
    children[0] = nullptr;
    children[1] = nullptr;
  }
 
  void UpdateAABB(float margin) {
    if (IsLeaf()) {
      // make fat AABB
      const Vec2 marginVec(margin, margin);
      aabb.minPoint = data->minPoint - marginVec;
      aabb.maxPoint = data->maxPoint + marginVec;
    }
    else
      // make union of child AABBs of child nodes
      aabb = 
        children[0]->aabb.Union(children[1]->aabb);
  }
 
  Node *GetSibling(void) const {
    return this == parent->children[0] ? parent->children[1] : parent->children[0];
  }
};