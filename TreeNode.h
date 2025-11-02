
#ifndef HUFFMANDECODER_TREENODE_H
#define HUFFMANDECODER_TREENODE_H
#pragma once
#include <string>

struct TreeNode {
    std::string word;
    std::string key_word;
    size_t count = 1;
    TreeNode* left = nullptr;
    TreeNode* right = nullptr;

    // Leaf
    explicit TreeNode(std::string w, std::size_t f=1)
        : word(std::move(w)), key_word(word), count(f) {}

    // Internal
    // FOR USE IN BUILDING OF HUFFMAN TREE (PART 3)
    TreeNode(TreeNode* L, TreeNode* R)
        : word(""),
          key_word(std::min(L->key_word, R->key_word)),
          count(L->count + R->count),
          left(L), right(R) {}
};

#endif //HUFFMANDECODER_TREENODE_H