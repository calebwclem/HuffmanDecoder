#include "TreeNode.h"
#include "HuffmanTree.h"
#include <ostream>
#include <istream>
#include <string>
#include <sstream>
#include <vector>
#include <utility>

HuffmanTree::~HuffmanTree() {
    destroy(root_);
    root_ = nullptr;
}

bool HuffmanTree::insertPath(TreeNode*& root,
                             const std::string& word,
                             const std::string& code,
                             std::string& err)
{
    if (!root) root = new TreeNode();

    TreeNode* cur = root;
    for (size_t i = 0; i < code.size(); ++i) {
        const char b = code[i];
        if (b != '0' && b != '1') { err = "Header contains a non-binary code bit."; return false; }

        TreeNode*& next = (b == '0') ? cur->left : cur->right;
        const bool last = (i + 1 == code.size());

        if (last) {
            if (next != nullptr) { // duplicate code or prefix collision
                err = "Header has duplicate code or prefix conflict.";
                return false;
            }
            next = new TreeNode();
            next->word = word; // make it a leaf
        }
        else {
            if (next == nullptr) {
                next = new TreeNode(); // create internal node to continue path
            } else if (isLeaf(next)) {   // can't descend “through” a leaf
                err = "Header has prefix conflict (existing code is prefix of another).";
                return false;
            }
            cur = next;
        }
    }
    return true;
}//End insertPath

bool HuffmanTree::buildFromHeader(const std::vector<std::pair<std::string,std::string>>& header_pairs,
                         std::string& err){
    //Start from clean tree
    destroy(root_);
    root_ = nullptr;

    if (header_pairs.empty())
        return true; // empty tree == empty output; valid

    for (const auto& [word, code] : header_pairs) {
        if (word.empty() || code.empty()) {
            err = "Header has empty word or code.";
            return false;
        }
        if (!insertPath(root_, word, code, err))
            return false;
    }
    return true;
}//End buildFromHeader

bool HuffmanTree::decode(std::istream& code_in,
                         std::ostream& out_tokens,
                         std::string& err) const
{
    // Trivial empty case
    if (!root_) return true;

    // If root is a single leaf, the only valid bit is '0' and each '0' emits the word.
    const bool single_leaf = isLeaf(root_);

    TreeNode* cur = root_;
    char ch;
    while (code_in.get(ch)) {
        if (ch == '\n' || ch == '\r') continue; // ignore line breaks (80-col wrapping)
        if (ch != '0' && ch != '1') { err = "Code file contains a non-binary character."; return false; }

        if (single_leaf) {
            if (ch != '0') { err = "Invalid bit for single-symbol code (expected only '0')."; return false; }
            out_tokens << root_->word << '\n';
            if (!out_tokens) { err = "Failed writing decoded token."; return false; }
            continue;
        }

        // Traverse
        cur = (ch == '0') ? cur->left : cur->right;
        if (!cur) { err = "Bitstream leads to a non-existent branch (invalid code path)."; return false; }

        if (isLeaf(cur)) {
            out_tokens << cur->word << '\n';
            if (!out_tokens) { err = "Failed writing decoded token."; return false; }
            cur = root_; // reset for next word
        }
    }//End while

    // If we ended mid-path (not at root), the bitstream terminated early
    if (!single_leaf && cur != root_) {
        err = "Bitstream ended before reaching a leaf (truncated code).";
        return false;
    }
    return true;
}//End decode
