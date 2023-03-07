#ifndef HUFFMANTREE_H
#define HUFFMANTREE_H

#include <queue>
#include <string>

namespace huffman
{
	enum Parent // enum for parent nodes
	{
		PARENT = '*',
	};

	struct Node
	{
		char   symbol{};
		size_t freq{};
		Node*  left{nullptr};
		Node*  right{nullptr};
		explicit Node(char symbol, int freq = 0)
		    : symbol(symbol)
		    , freq(freq){};
	};

	struct data
	{
		Node*        node{};
		std::string  line{};
		std::string* word{};
	};

	struct compare
	{
		bool operator()(const Node* n1, const Node* n2)
		{
			if (n1->freq == n2->freq)
			{
				if (n1->symbol == PARENT && n2->symbol == PARENT)
				{
					if (n1->right->symbol < n2->right->symbol)
						return true;
					else if (n1->right->symbol > n2->right->symbol)
						return false;
				}
				else if (n1->symbol == PARENT)
					return compare()(n1->right, n2);
				else if (n2->symbol == PARENT)
					return compare()(n1, n2->right);
				return !(n1->symbol < n2->symbol);
			}
			return n1->freq > n2->freq;
		}
	};
	using priority = std::priority_queue<Node*, std::vector<Node*>, compare>;
	void   insert(priority& pq, const char& symbol, const int& freq);
	size_t merge_tree(priority& pq);
	void   preorder(Node* cu, std::string code = "");
	// void*  decode(void* void_ptr);
	char decode(std::string& line, huffman::Node& top);
	void   deleteNodes(Node* cu);

} // namespace huffman

#endif