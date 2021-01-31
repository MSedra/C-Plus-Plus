/**
 * @file
 * @brief [Persistent segment tree with range updates (lazy propagation)](https://en.wikipedia.org/wiki/Persistent_data_structure)
 *
 * @details
 * A normal segment tree facilitates making point updates and range queries in logarithmic time.
 * Lazy propagation preserves the logarithmic time with range updates.
 * So, a segment tree with lazy propagation enables doing range updates and range queries in logarithmic time, but it doesn't save any information about itself before the last update.
 * A persistent data structure always preserves the previous version of itself when it is modified.
 * That is, a new version of the segment tree is generated after every update.
 * It saves all previous versions of itself (before every update) to facilitate doing range queries in any version.
 * More memory is used ,but the logarithmic time is preserved because the new version points to the same nodes, that the previous version points to, that are not affected by the update.
 * That is, only the nodes that are affected by the update and their ancestors are copied. The rest is copied using lazy propagation in the next queries.
 * Thus preserving the logarithmic time because the number of nodes copied after any update is logarithmic.
 *
 * @author [Magdy Sedra](https://github.com/MSedra)
 */
#include<iostream> /// for IO operations
#include<vector>   /// for std::vector
#include <memory>  /// to manage dynamic memory

/**
 * @namespace range_queries
 * @brief Range queries algorithms
 */
namespace range_queries {

/**
 * @brief Range query here is range sum, but the code can be modified to make different queries like range max or min.
 */
    class perSegTree {
    private:
        class Node {
        public:
            std::shared_ptr<Node> left; /// pointer to the left node
            std::shared_ptr<Node> right; /// pointer to the right node
            int64_t val, prop; /// val is the value of the node (here equals to the sum of the leaf nodes children of that node), prop is the value to be propagated/added to all the leaf nodes children of that node

            Node() {
                val = prop = 0;
                left = right = nullptr;
            }
        };

        int n; /// number of elements/leaf nodes in the segment tree
        std::vector<std::shared_ptr<Node>> ptrs; /// ptrs[i] holds a root pointer to the segment tree after the ith update. ptrs[0] holds a root pointer to the segment tree before any updates
        std::vector<int> vec; /// values of the leaf nodes that the segment tree will be constructed with

        /**
         * @brief Creating a new node with the same values of curr node
         * @param curr node that would be copied
         * @return the new node
         */
        std::shared_ptr<Node> newKid(std::shared_ptr<Node> const &curr) {
            auto newNode = std::make_shared<Node>(Node());
            newNode->left = curr->left;
            newNode->right = curr->right;
            newNode->prop = curr->prop;
            newNode->val = curr->val;
            return newNode;
        }

        /**
         * @brief If there is some value to be propagated to the passed node, value is added to the node and the children of the node, if exist, are copied and the propagated value is also added to them
         * @param i the left index of the range that the passed node holds its sum
         * @param j the right index of the range that the passed node holds its sum
         * @param curr pointer to the node to be propagated
         * @return void
         */
        void lazy(const int &i, const int &j, std::shared_ptr<Node> const &curr) {
            if (!curr->prop) {
                return;
            }
            curr->val += (j - i + 1) * curr->prop;
            if (i != j) {
                curr->left = newKid(curr->left);
                curr->right = newKid(curr->right);
                curr->left->prop += curr->prop;
                curr->right->prop += curr->prop;
            }
            curr->prop = 0;
        }

        /**
         * @brief Constructing the segment tree with the early passed vector. Every call creates a node to hold the sum of the given range, set its pointers to the children, and set its value to the sum of the children's values
         * @param i the left index of the range that the created node holds its sum
         * @param j the right index of the range that the created node holds its sum
         * @return pointer to the newly created node
         */
        std::shared_ptr<Node> construct(const int &i, const int &j) {
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

        /**
         * @brief Doing range update, checking at every node if it has some value to be propagated. All nodes affected by the update are copied and propagation value is added to the leaf of them
         * @param i the left index of the range that the passed node holds its sum
         * @param j the right index of the range that the passed node holds its sum
         * @param l the left index of the range to be updated
         * @param r the right index of the range to be updated
         * @param value the value to be added to every element whose index x satisfies l<=x<=r
         * @param curr pointer to the current node, which has value = the sum of elements whose index x satisfies i<=x<=j
         * @return pointer to the current newly created node
         */
        std::shared_ptr<Node> update(const int &i, const int &j, const int &l, const int &r, const int &value, std::shared_ptr<Node> const &curr) {
            lazy(i, j, curr);
            if (i >= l && j <= r) {
                std::shared_ptr<Node> newNode = newKid(curr);
                newNode->prop += value;
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
            return newNode;
        }

        /**
         * @brief Querying the range from index l to index r, checking at every node if it has some value to be propagated. Current node's value is returned if its range is completely inside the wanted range, else 0 is returned
         * @param i the left index of the range that the passed node holds its sum
         * @param j the right index of the range that the passed node holds its sum
         * @param l the left index of the range whose sum should be returned as a result
         * @param r the right index of the range whose sum should be returned as a result
         * @param curr pointer to the current node, which has value = the sum of elements whose index x satisfies i<=x<=j
         * @return sum of elements whose index x satisfies l<=x<=r
         */
        int64_t query(const int &i, const int &j, const int &l, const int &r, std::shared_ptr<Node> const &curr) {
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

        /**
         * public methods that can be used directly from outside the class. They call the private functions that do all the work
         */
    public:
        perSegTree() {
            n = 0;
        }

        /**
         * @brief Constructing the segment tree with the values in the passed vector. Returned root pointer is pushed in the pointers vector to have access to the original version if the segment tree is updated
         * @param vec vector whose values will be used to build the segment tree
         * @return void
         */
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

        /**
         * @brief Doing range update by passing the left and right indices of the range as well as the value to be added.
         * @param l the left index of the range to be updated
         * @param r the right index of the range to be updated
         * @param value the value to be added to every element whose index x satisfies l<=x<=r
         * @return void
         */
        void update(const int &l, const int &r, const int &value) // all elements from index "l" to index "r" would by updated by "value", "l" and "r" are 0 indexed
        {
            ptrs.push_back(update(0, n - 1, l, r, value,ptrs[ptrs.size() - 1])); // saving the root pointer to the new segment tree
        }

        /**
         * @brief Querying the range from index l to index r, getting the sum of the elements whose index x satisfies l<=x<=r
         * @param l the left index of the range whose sum should be returned as a result
         * @param r the right index of the range whose sum should be returned as a result
         * @param version the version to query on. If equals to 0, the original segment tree will be queried
         * @return sum of elements whose index x satisfies l<=x<=r
         */
        int64_t query(const int &l, const int &r, const int &version) // querying the range from "l" to "r" in a segment tree after "version" updates, "l" and "r" are 0 indexed
        {
            return query(0, n - 1, l, r, ptrs[version]);
        }

        /**
         * @brief Getting the number of versions after updates so far which is equal to the size of the pointers vector
         * @return the number of versions
         */
        int size() // returns the number of segment trees (versions) , the number of updates done so far = returned value - 1 ,because one of the trees is the original segment tree
        {
            return ptrs.size();
        }
    };
} // namespace range_queries

void test() {
    std::vector<int> arr = {-5, 2, 3, 11, -2, 7, 0, 1};
    range_queries::perSegTree tree;
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
}

int main() {
    test(); // run self-test implementations
    return 0;
}



