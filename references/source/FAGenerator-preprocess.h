#ifndef FAGENERATOR_PREPROCESS_EXAMPLE_H
#define FAGENERATOR_PREPROCESS_EXAMPLE_H

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

class FAGenerator
{
private:
    std::vector<std::string> reExps;
    std::vector<std::pair<std::string, int>> processedExp2id;    // 处理后的正则表达式和对应的 tokenId 的映射
    std::map<std::string, int> keyword2id;      // 关键字
    std::map<std::string, std::string> macro2exp;       // 宏定义到正则表达式的映射
    int idTokenId = -1; 
    std::set<int> commentTokenIds;

    void preprocessReExp();
    std::string expandExpression(const std::string& curExp, std::set<std::string>& active);
};

#endif
