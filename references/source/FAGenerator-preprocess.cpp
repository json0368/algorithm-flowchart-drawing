#include "FAGenerator-preprocess.h"
#include <cctype>
#include <stdexcept>

// 判断字符串对应位置是否是转义字符
bool isEscaped(const std::string& text, int pos) {
    int count = 0;
    while (pos > 0 && text[--pos] == '\\') count++;
    return count % 2 != 0;
}


// 寻找与右括号对应的左括号位置，返回 -1 表示没有找到
int findLeftBracket(const std::string& str, int right) {
    int depth = 0;
    for (int i = right; i >= 0; i--) {
        if (isEscaped(str, i)) continue;
        if (str[i] == ')') {
            depth++;
        } else if (str[i] == '(') {
            depth--;
            if (depth == 0) {
                return i;
            }
        }
    }
    return -1;
}


// 将表达式中的宏定义递归替换为对应的正则表达式，并检查循环引用
std::string FAGenerator::expandExpression(
    const std::string& curExp,
    std::set<std::string>& active) {

    std::string replacedExp;
    int idx = 0;

    while (idx < curExp.size()) {
        // 转义字符原样保留
        if (curExp[idx] == '\\' && idx + 1 < curExp.size()) {
            replacedExp += curExp[idx];
            replacedExp += curExp[idx + 1];
            idx += 2;
            continue;
        }

        // [] 中的内容不进行宏替换
        if (curExp[idx] == '[') {
            int right = idx + 1;
            while (right < curExp.size() &&
                (curExp[right] != ']' || isEscaped(curExp, right)))
                right++;

            if (right == curExp.size())
                throw std::runtime_error("缺少 ]");

            replacedExp += curExp.substr(idx, right - idx + 1);
            idx = right + 1;
            continue;
        }

        // 将宏定义替换为对应的正则表达式
        if (curExp[idx] == '_' || isalpha(curExp[idx])) {
            // 找到宏定义的完整单词
            int k = idx + 1;
            while (k < curExp.size() && (curExp[k] == '_' || isalnum(curExp[k]))) {
                k++;
            }
            // 提取宏定义单词并进行替换
            std::string word = curExp.substr(idx, k - idx);
            if (macro2exp.find(word) != macro2exp.end()) {
                // replacedExp += "(" + macro2exp[word] + ")";
                if (!active.insert(word).second) {
                    throw std::runtime_error("宏循环引用：" + word);
                }
                replacedExp += "(" + expandExpression(macro2exp.at(word), active) + ")";
                active.erase(word);
            } else {
                replacedExp += word;
            }
            idx = k;
        } else {
            // 普通字符直接插入
            replacedExp += curExp[idx];
            idx++;
        }
    }
    return replacedExp;
}


// 预处理正则表达式
void FAGenerator::preprocessReExp() {
    int n = reExps.size();

    // 去掉空格
    for (int i = 0; i < n; i++) {
        int len = reExps[i].size();
        for (int j = 0; j < len; j++) {
            if (reExps[i][j] == ' ') {
                for (int k = j; k < len - 1; k++) {
                    reExps[i][k] = reExps[i][k + 1];
                }
                len--;
                j--;
            }
        }
        reExps[i].resize(len);
    }

    // 处理宏定义，并拆分多条正则表达式
    std::vector<std::pair<std::string, int>> exp2tokenId;
    for (int i = 0; i < n; i++) {
        std::string line = reExps[i];
        int len = reExps[i].size();
        int eqIdx = line.find('=');

        if (eqIdx == std::string::npos) {
            continue;
        }

        std::string name = line.substr(0, eqIdx);
        std::string exp = line.substr(eqIdx + 1, len - eqIdx - 1);
        macro2exp[name] = exp;

        // 将表达式左侧转化为小写，方便进行匹配
        std::string lowerName = name;
        for (char& c : lowerName) {
            c = std::tolower(c);
        }

        std::string tokenId;
        int branch = 0;
        int tmpIdx = name.find('_');
        if (lowerName.find("keyword") == 0) {
            // 关键字规则
            tokenId = name.substr(7); // 跳过 "keyword" 前缀
            branch = 1; 
            if (tokenId.empty()) throw std::runtime_error("关键词必须提供编码！");   
        } else if (lowerName.find("comment_") == 0) {
            // 注释规则
            tokenId = name.substr(8); // 跳过 "comment_" 前缀
            branch = 2;  
            if (tokenId.empty()) throw std::runtime_error("注释必须提供编码！");
        } else if (tmpIdx != std::string::npos) {
            // 普通宏定义
            tokenId = name.substr(tmpIdx + 1, name.size() - tmpIdx - 1);
            branch = 3;
            if (tokenId.empty()) continue;  // _ 后面没有数字编码，跳过
            if (lowerName.find("id_") == 0) {
                // 记录标识符的编码
                idTokenId = std::stoi(tokenId);
            }
        } else {
            continue;  // 其他没有数字编码的直接跳过
        }
                
        if (tokenId.back() == 'L') {
            int baseId = std::stoi(tokenId.substr(0, tokenId.size() - 1));
            std::string part;
            for (char c : exp) {
                if (c == '|') {
                    // 记录 tokenid 
                    if (branch == 1) {
                        keyword2id[part] = baseId;
                    } else if (branch == 2) {
                        commentTokenIds.insert(baseId);
                    }
                    
                    if (branch != 1) exp2tokenId.push_back({part, baseId});
                    part.clear();
                    baseId++;
                } else {
                    part += c;
                }
            }
            if (!part.empty()) {
                if (branch == 1) {
                    keyword2id[part] = baseId;
                } else if (branch == 2) {
                    commentTokenIds.insert(baseId);
                }
                if (branch != 1) exp2tokenId.push_back({part, baseId});
            }
        } else {
            int id = std::stoi(tokenId);
            if (branch == 1) {
                keyword2id[exp] = id;
            } else if (branch == 2) {
                commentTokenIds.insert(id);
            }
            if (branch != 1) exp2tokenId.push_back({exp, id});
        }
    }

    for (int i = 0; i < exp2tokenId.size(); i++) {
        std::string curExp = exp2tokenId[i].first;
        int tokenId = exp2tokenId[i].second;
        std::string replacedExp = "";

        // 处理宏定义替换
        std::set<std::string> active;  // 用于检测宏循环引用
        replacedExp = expandExpression(curExp, active);

        // 去除 []，替换成 (a|b|c) 这种形式
        for (int j = 0; j < replacedExp.size(); j++) {
            if (replacedExp[j] == '\\') {  // 转义字符
                j++;
                continue;
            }
            if (replacedExp[j] != '[') continue;

            int left = j;
            int right = left + 1;
            while (right < replacedExp.size() &&
                (replacedExp[right] != ']' || isEscaped(replacedExp, right)))
                right++;
            if (right == replacedExp.size()) throw std::runtime_error("字符类缺少 ]");
            
            std::string between = replacedExp.substr(left + 1, right - left - 1);
            if (between.empty()) throw std::runtime_error("字符类不能为空");

            std::string expanded;
            int k = 0;
            while (k < between.size()) {
                int first = (unsigned char)between[k++];

                if (first == '\\') {
                    first = (unsigned char)between[k++];
                    if (first == 'n') first = '\n';
                    else if (first == 't') first = '\t';
                    else if (first == 'r') first = '\r';
                }

                // 普通字符只展开自身
                int last = first;

                // 范围字符，例如 a-z
                if (k + 1 < between.size() && between[k] == '-') {
                    k++;
                    last = (unsigned char)between[k++];
                    if (last == '\\') {
                        last = (unsigned char)between[k++];
                        if (last == 'n') last = '\n';
                        else if (last == 't') last = '\t';
                        else if (last == 'r') last = '\r';
                    }
                    if (first > last) throw std::runtime_error("字符范围起点大于终点");
                }

                for (int c = first; c <= last; c++) {
                    // 如果是特殊字符，需要加上转义符
                    if (std::string("\\|*+?()[]~&#").find(char(c)) != std::string::npos) {
                        expanded += '\\';
                    }
                    expanded += char(c);
                    expanded += '|';
                }
            }

            if (!expanded.empty()) {
                expanded.pop_back(); // 去掉最后一个 '|'
                std::string replacement = "(" + expanded + ")";
                replacedExp.replace(left, right - left + 1, replacement);
                j = left + replacement.size() - 1;
            }
        }

        // 把 ~a 改为 (~a)
        for (int j = replacedExp.size() - 1; j >= 0; j--) {
            if (replacedExp[j] != '~' || isEscaped(replacedExp, j)) {
                continue;
            }

            int k = j + 1;
            if (k == replacedExp.size()) {
                throw std::runtime_error("~ 后缺少字符");
            }
            if (replacedExp[k] == '(') {
                // 寻找右括号
                int depth = 0;
                do {
                    if (replacedExp[k] == '\\') {
                        // 跳过转义字符
                        if (k + 1 == replacedExp.size())
                            throw std::runtime_error("转义符后缺少字符");
                        k += 2;
                        continue;
                    }

                    if (replacedExp[k] == '(') ++depth;
                    else if (replacedExp[k] == ')') --depth;
                    ++k;
                } while (k < replacedExp.size() && depth > 0);

                if (depth != 0) throw std::runtime_error("括号不匹配");
            } else {
                k += replacedExp[k] == '\\' ? 2 : 1;
                if (k > replacedExp.size()) throw std::runtime_error("~ 后缺少字符");
            }
            replacedExp.insert(k, ")");
            replacedExp.insert(j, "(");
        }

        // 把正闭包 + 改为闭包 *
        for (int j = 0; j < replacedExp.size(); j++) {
            if (replacedExp[j] == '\\') {
                j++;
                continue;
            }

            if (replacedExp[j] == '+') {
                replacedExp[j] = '*';
                if (j == 0) {
                    // 如果 + 是第一个字符，直接删掉
                    replacedExp.erase(j, 1);
                    j--;
                    continue;
                }

                if (isEscaped(replacedExp, j - 1)) {
                    // 前面是转义字符，重复整个转义序列
                    std::string block = replacedExp.substr(j - 2, 2);
                    replacedExp.insert(j - 2, block);
                    j += 2;
                } else if (replacedExp[j - 1] == ')') {
                    // 前面是 )，需要将整个 () 包裹的内容重复一遍
                    int left = findLeftBracket(replacedExp, j - 1);
                    std::string block = replacedExp.substr(left, j - left);
                    replacedExp.insert(left, block);
                    j += block.size();
                } else {
                    // 前面是普通字符，直接重复前一个字符
                    replacedExp.insert(j, 1, replacedExp[j - 1]);
                    j++;
                }
            }
        }

        // 把 a? 改为 (a|#)
        for (int j = 0; j < replacedExp.size(); j++) {
            if (replacedExp[j] == '\\') {
                j++;
                continue;
            }

            if (replacedExp[j] == '?') {
                if (j == 0) {
                    // 直接去掉 ?
                    replacedExp.erase(j, 1);
                    j--;
                    continue;
                }

                replacedExp[j] = '|';
                if (isEscaped(replacedExp, j - 1)) {
                    // 前面是转义字符，重复整个转义序列
                    std::string block = replacedExp.substr(j - 2, 2);
                    replacedExp.insert(j - 2, "(");
                    replacedExp.insert(j + 2, "#)");
                    j += 3; // 跳过新插入的字符
                } else if (replacedExp[j - 1] == ')') {
                    // 前面是括号，需要将整个括号包裹的内容提取出来
                    int left = findLeftBracket(replacedExp, j - 1);
                    std::string block = replacedExp.substr(left, j - left);
                    replacedExp.insert(left, "(");
                    replacedExp.insert(j + 2, "#)");
                    j += 3; // 跳过新插入的字符
                } else {
                    // 前面是普通字符，直接将其包裹起来
                    replacedExp.insert(j - 1, "(");
                    replacedExp.insert(j + 2, "#)");
                    j += 3; // 跳过新插入的字符
                }
            }
        }

        processedExp2id.push_back({replacedExp, tokenId});
    }
}
