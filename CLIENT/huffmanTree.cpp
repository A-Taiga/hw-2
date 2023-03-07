#include "huffmanTree.h"
#include <iostream>
#include <sstream>
#include <vector>

void huffman::insert(priority& pq, const char& symbol, const int& freq)
{
	pq.push(new Node(symbol, freq));
}

// create the parent node
// add the top of the queue to the right node of parent and pop
// add the top of the queue to the left node of parent and pop
// add parents child nodes total frequencies to its own
// push parent to priroity queue
size_t huffman::merge_tree(priority& pq)
{
	while (pq.size() != 1)
	{
		auto* parent = new Node(PARENT);
		parent->left = pq.top();
		pq.pop();
		parent->right = pq.top();
		pq.pop();
		parent->freq = parent->left->freq + parent->right->freq;
		pq.push(parent);
	}
	return pq.top()->freq;
}

// using preorder traversal get each symbol binary representation
void huffman::preorder(Node* cu, std::string code)
{
	if (cu != nullptr)
	{
		preorder(cu->left, code + "0");
		preorder(cu->right, code + "1");

		if (cu->symbol != PARENT)
		{
			std::cout << "Symbol: " << cu->symbol << ", Frequency: " << cu->freq
			          << ", Code: " << code << std::endl;
		}
	}
}
void* huffman::decode(void* void_ptr)
{
	huffman::data*     packet  = (huffman::data*)void_ptr;
	huffman::Node*     current = packet->node;
	std::string        code{};
	size_t             pos{};
	std::istringstream ss{packet->line};
	ss >> code;
	for (size_t i = 0; i < code.length(); i++)
	{

		if (code[i] == '0')
			current = current->left;
		else if (code[i] == '1')
			current = current->right;
	}
	while (ss >> pos)
		packet->word->at(pos) = current->symbol;

	return nullptr;
}
// delete pointers
void huffman::deleteNodes(Node* cu)
{
	if (cu != nullptr)
	{
		deleteNodes(cu->left);
		deleteNodes(cu->right);
		if (cu != nullptr)
		{
			delete cu;
			cu = nullptr;
		}
	}
}