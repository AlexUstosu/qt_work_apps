#include "tagshighlighter.h"

TagsHighlighter::TagsHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;


    infoFormat.setFontWeight(QFont::Bold);
    infoFormat.setForeground(QColor("#97CC94"));
    rule.pattern = QRegularExpression("INFO");
    rule.format = infoFormat;
    highlightingRules.append(rule);

    warningFormat.setFontWeight(QFont::Bold);
    warningFormat.setForeground(QColor("#95124F"));
    rule.pattern = QRegularExpression("WARNING");
    rule.format = warningFormat;
    highlightingRules.append(rule);

    dataFormat.setFontWeight(QFont::Bold);
    dataFormat.setForeground(QColor("#CCC994"));
    rule.pattern = QRegularExpression("DATA");
    rule.format = dataFormat;
    highlightingRules.append(rule);
}
void TagsHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
