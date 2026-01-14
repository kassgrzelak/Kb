class TrieNode:
    def __init__(self):
        self.children = {}
        self.word = None
        self.terminal_count = 0


class Trie:
    def __init__(self):
        self.root = TrieNode()

    def insert(self, word):
        node = self.root
        for ch in word:
            node = node.children.setdefault(ch, TrieNode())
        node.word = word

    def build_from_keywords(self, keywords):
        for word in keywords:
            self.insert(word)
        self._compute_terminal_counts(self.root)

    def _compute_terminal_counts(self, node):
        count = 1 if node.word else 0
        for child in node.children.values():
            count += self._compute_terminal_counts(child)
        node.terminal_count = count
        return count

    def generate_cpp(self):
        return "\n".join(self._emit_level(self.root, depth=0, indent=4))

    def _emit_level(self, node, depth, indent):
        lines = []
        ind = " " * indent

        for ch, child in sorted(node.children.items()):
            lines.append(f"{ind}case '{ch}':")

            # Unique keyword under this prefix
            if child.terminal_count == 1:
                word = self._find_word(child)
                token = f"TOKEN_{word.upper()}"
                suffix = word[depth + 1:]

                lines.append(
                    f"{ind}  return checkKeyword("
                    f"{depth + 1}, {len(suffix)}, "
                    f"\"{suffix}\", {token});"
                )
                continue

            # Multiple keywords share this prefix → nest
            lines.append(
                f"{ind}  if (scanner.current - scanner.start > {depth + 1})"
            )
            lines.append(f"{ind}  {{")
            lines.append(
                f"{ind}    switch (scanner.start[{depth + 1}])"
            )
            lines.append(f"{ind}    {{")
            lines.extend(
                self._emit_level(child, depth + 1, indent + 6)
            )
            lines.append(f"{ind}    }}")
            lines.append(f"{ind}  }}")
            lines.append(f"{ind}  break;")

        return lines

    def _find_word(self, node):
        if node.word:
            return node.word
        for child in node.children.values():
            w = self._find_word(child)
            if w:
                return w
        return None


keywords = ["inc", "dec",
            "if", "else",
            "and", "or", "true", "false",
            "class", "super", "this",
            "for", "while", "func",
            "print", "return", "var",
            "zilch", "inf"]

trie = Trie()
trie.build_from_keywords(keywords)

print(trie.generate_cpp())

