#ifndef TAGSHIGHLIGHTER_H
#define TAGSHIGHLIGHTER_H

#include <QObject>
#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>

class TagsHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    TagsHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat infoFormat;
    QTextCharFormat warningFormat;
    QTextCharFormat dataFormat;
};

#endif // TAGSHIGHLIGHTER_H
