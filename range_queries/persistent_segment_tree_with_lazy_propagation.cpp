#include<iostream>
#include<vector>
#include <memory>

// query -> range sum
class perSegTree {
private:
    class Node {
    public:
        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;
        int64_t val, prop;
        int version;

        Node() {
            val = prop = version = 0;
            left = right = nullptr;
        }
    };

    int n; // number of elements in the segment tree
    std::vector<std::shared_ptr<Node>> ptrs; // ptrs[i] holds a root pointer to the segment tree after the ith update
    std::vector<int> vec;

    std::shared_ptr<Node> newKid(std::shared_ptr<Node> const &curr) {
        //std::shared_ptr<Node> newNode(new Node());
        auto newNode = std::make_shared<Node>(Node());
        newNode->left = curr->left;
        newNode->right = curr->right;
        newNode->prop = curr->prop;
        newNode->val = curr->val;
        return newNode;
    }

    void lazy(int i, int j, std::shared_ptr<Node> const &curr) {
        if (!curr->prop) {
            return;
        }
        curr->val += (j - i + 1) * curr->prop;
        if (i != j) {
            curr->left = newKid(curr->left);
            curr->right = newKid(curr->right);
            curr->left->prop += curr->prop;
            curr->right->prop += curr->prop;
            curr->left->version = curr->right->version = curr->version;
        }
        curr->prop = 0;
    }

    std::shared_ptr<Node> construct(int i, int j) {
        auto newNode = std::make_shared<Node>(Node());
        if (i == j) {
            newNode->val = vec[i];
        } else {
            int mid = i + (j - i) / 2;
            auto leftt = construct(i, mid);
            auto right = construct(mid + 1, j);
            newNode->val = leftt->val + right->val;
            newNode->left = leftt;
            newNode->right = right;
        }
        return newNode;
    }

    std::shared_ptr<Node> update(int i, int j, int l, int r, int value, std::shared_ptr<Node> const &curr) {
        lazy(i, j, curr);
        if (i >= l && j <= r) {
            std::shared_ptr<Node> newNode = newKid(curr);
            newNode->prop += value;
            newNode->version = ptrs.size();
            lazy(i, j, newNode);
            return newNode;
        }
        if (i > r || j < l) {
            return curr;
        }
        auto newNode = std::make_shared<Node>(Node());
        int mid = i + (j - i) / 2;
        newNode->left = update(i, mid, l, r, value, curr->left);
        newNode->right = update(mid + 1, j, l, r, value, curr->right);
        newNode->val = newNode->left->val + newNode->right->val;
        newNode->version = ptrs.size();
        return newNode;
    }

    int64_t query(int i, int j, int l, int r, std::shared_ptr<Node> const &curr) {
        lazy(i, j, curr);
        if (j < l || r < i) {
            return 0;
        }
        if (i >= l && j <= r) {
            return curr->val;
        }
        int mid = i + (j - i) / 2;
        return query(i, mid, l, r, curr->left) + query(mid + 1, j, l, r, curr->right);
    }

public:
    perSegTree() {
        n = 0;
    }

    void construct(const std::vector<int> &vec) // the segment tree will be built from the values in "vec", "vec" is 0 indexed
    {
        if (vec.empty()) {
            return;
        }
        n = vec.size();
        this->vec = vec;
        auto root = construct(0, n - 1);
        ptrs.push_back(root);
    }

    void update(int l, int r, int value) // all elements from index "l" to index "r" would by updated by "value", "l" and "r" are 0 indexed
    {
        ptrs.push_back(update(0, n - 1, l, r, value,
                              ptrs[ptrs.size() - 1])); // saving the root pointer to the new segment tree
    }

    int64_t query(int l, int r, int version) // querying the range from "l" to "r" in a segment tree after "version" updates, "l" and "r" are 0 indexed
    {
        return query(0, n - 1, l, r, ptrs[version]);
    }

    int size() // returns the number of segment trees (versions) , the number of updates done so far = returned value - 1 ,because one of the trees is the original segment tree
    {
        return ptrs.size();
    }
};

int main() {

    std::vector<int> arr = {-5, 2, 3, 11, -2, 7, 0, 1};
    perSegTree tree;
    std::cout << "Elements before any updates are {";
    for (int i = 0; i < arr.size(); ++i) {
        std::cout << arr[i];
        if (i != arr.size() - 1) {
            std::cout << ",";
        }
    }
    std::cout << "}\n";
    tree.construct(arr); // constructing the original segment tree (version = 0)
    std::cout << "Querying range sum on version 0 from index 2 to 4 = 3+11-2 = " << tree.query(2, 4, 0) << '\n';
    std::cout << "Subtract 7 from all elements from index 1 to index 5 inclusive\n";
    tree.update(1, 5, -7); // subtracting 7 from index 1 to index 5
    std::cout << "Elements of the segment tree whose version = 1 (after 1 update) are {";
    for (int i = 0; i < arr.size(); ++i) {
        std::cout << tree.query(i, i, 1);
        if (i != arr.size() - 1) {
            std::cout << ",";
        }
    }
    std::cout << "}\n";
    std::cout << "Add 10 to all elements from index 0 to index 7 inclusive\n";
    tree.update(0, 7, 10); // adding 10 to all elements
    std::cout << "Elements of the segment tree whose version = 2 (after 2 updates) are {";
    for (int i = 0; i < arr.size(); ++i) {
        std::cout << tree.query(i, i, 2);
        if (i != arr.size() - 1) {
            std::cout << ",";
        }
    }
    std::cout << "}\n";
    std::cout << "Number of segment trees (versions) now = " << tree.size() << '\n';
    std::cout << "Querying range sum on version 0 from index 3 to 5 = 11-2+7 = " << tree.query(3, 5, 0) << '\n';
    std::cout << "Querying range sum on version 1 from index 3 to 5 = 4-9+0 = " << tree.query(3, 5, 1) << '\n';
    return 0;
}