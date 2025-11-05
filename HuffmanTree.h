
#ifndef HUFFMANDECODER_HUFFMANTREE_H
#define HUFFMANDECODER_HUFFMANTREE_H

#include <string>
#include "TreeNode.h"
#include <utility>
#include <vector>


class HuffmanTree {
public:
    HuffmanTree() = default;
    ~HuffmanTree();

    // Build the decoding tree from (word, code) pairs parsed from .hdr.
    // Returns true on success; false and sets err on invalid header.
    bool buildFromHeader(const std::vector<std::pair<std::string,std::string>>&, std::string&);

    // Decode .code into out_tokens (one word per line). Newlines in .code are ignored.
    // Returns true on success; false and sets err on invalid bitstream or I/O failure.
    bool decode(std::istream& code_in, std::ostream& out_tokens, std::string& err) const;

private:
    TreeNode* root_ = nullptr;

    static void destroy(TreeNode* n) noexcept {
        if (!n)
            return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }
    static bool isLeaf(const TreeNode* n) {
        return n && !n->left && !n->right && !n->word.empty();
    }

    // Insert a single (word, code) path into the tree. Returns false if invalid.
    static bool insertPath(TreeNode*& root,
                           const std::string& word,
                           const std::string& code,
                           std::string& err);
};

#endif //HUFFMANDECODER_HUFFMANTREE_H