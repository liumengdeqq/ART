//===- TextEditorWidget.cpp - ART-GUI TextEditorWidget Tab ---------------*- C++ -*-===//
//
//                     ANDROID REVERSE TOOLKIT
//
// This file is distributed under the GNU GENERAL PUBLIC LICENSE
// V3 License. See LICENSE.TXT for details.
//
//===---------------------------------------------------------------------===//
#include "TextEditorWidget.h"

#include "EditorTab/SmaliEditor.h"
#include "EditorTab/CodeEditor.h"
#include "SmaliOpInformation.h"


#include <utils/Configuration.h>
#include <BookMark/BookMarkManager.h>

#include <QTextStream>
#include <QtConcurrent/QtConcurrent>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QVBoxLayout>
#include <QFileInfo>
#include <QWidget>


TextEditorWidget::TextEditorWidget(QWidget *parent) :
        QWidget(parent),
        mFileEdit(Q_NULLPTR)
{
    mFileChangedTimer = new QTimer(this);
    mFileReloadTimer = new QTimer(this);
    mFileReloadTimer->setSingleShot (true);
    mFindWidget = new FindWidget(this);

    mFileChangedTimer->start(60*1000);

    connect(mFileChangedTimer, &QTimer::timeout, [this](){
        if(mFileEdit->document()->isModified()) {
            if(saveFile()) {
                mFileEdit->document()->setModified(false);
            }
        }
    });
    connect(mFileReloadTimer, &QTimer::timeout, [this]() {
        if(notReload) {
            notReload = false;
            return;
        }
        reload ();
    });

    // FindWidget signal
    connect(mFindWidget, SIGNAL(find(QString,QTextDocument::FindFlags)),
            this, SLOT(onFind(QString,QTextDocument::FindFlags)));
    connect(mFindWidget, SIGNAL(replace(QString,QString)),
            this, SLOT(onReplace(QString,QString)));
    connect(mFindWidget, SIGNAL(replaceAll(QString,QString)),
            this, SLOT(onReplaceAll(QString,QString)));
    connect(mFindWidget, SIGNAL(closeWidget()),
            this, SLOT(onFindClose ()));

    mFindWidget->hide ();
    loadFromConfig();

}

TextEditorWidget::~TextEditorWidget()
{
    closeFile();
    saveToConfig();
}


bool TextEditorWidget::openFile(QString filePath, int iLine)
{
    if(!QFileInfo::exists(filePath)) {
        return false;
    }
    Q_ASSERT(mFileEdit == Q_NULLPTR && "invoke again?");
    if(filePath.endsWith(".smali")) {
        mFileEdit = new SmaliEditor(this);
    } else {
        mFileEdit = new CodeEditor(this);
    }
    if(!mFileEdit->openFile(filePath, iLine)) {
        mFileEdit->deleteLater();
        return false;
    }

    connect(mFileEdit, &QPlainTextEdit::textChanged, this, &TextEditorWidget::textChanged);

    mFindWidget->setFilePath(filePath);

    // setup editor layout

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->addWidget(mFileEdit);
    vLayout->addWidget(mFindWidget);
    if(filePath.endsWith(".smali")) {
        vLayout->addWidget(new SmaliOpInformation(mFileEdit));
    }

    setLayout(vLayout);
    return true;
}

bool TextEditorWidget::saveFile()
{
    if(!mFileEdit->saveFile()) {
        return false;
    }
    notReload = true;
    return true;
}

bool TextEditorWidget::reload()
{
    return mFileEdit->reload();
}

void TextEditorWidget::closeFile()
{
    saveFile();
    mFileEdit->deleteLater();
    mFileEdit = Q_NULLPTR;
}

void TextEditorWidget::loadFromConfig()
{
//    Configuration *config = Config();
}

void TextEditorWidget::saveToConfig()
{
//    Configuration *config = Config();
}


int TextEditorWidget::currentLine ()
{
    return mFileEdit->currentLine ();
}

void TextEditorWidget::textChanged()
{
}

void TextEditorWidget::onFindAction ()
{
    mFindWidget->show ();
    auto text = mFileEdit->textCursor ().selectedText ();
    if(!text.isEmpty ()) {
        mFindWidget->setFindText (text);
    }
    mFindWidget->setFindEditFocus ();
}

void TextEditorWidget::onFindClose ()
{
    mFindWidget->hide ();
    mFileEdit->setFocus ();
    QList<QTextEdit::ExtraSelection> extraSelections;
    extraSelections.push_front (mFileEdit->extraSelections ().front ());
    mFileEdit->setExtraSelections(extraSelections);
}

void TextEditorWidget::onFind(const QString &subString, QTextDocument::FindFlags options)
{
    highlightWord (subString, options);

    auto select = mFileEdit->textCursor ().selectedText ();
    if(select != subString) {
        auto cursor = mFileEdit->textCursor ();
        cursor.setPosition (cursor.selectionStart ());
        mFileEdit->setTextCursor (cursor);
    }
    // find current
    if(!mFileEdit->find (subString, options)) {
        // research from top or buttom
        auto cursor = mFileEdit->textCursor ();
        if(options == QTextDocument::FindBackward) {
            mFileEdit->moveCursor (QTextCursor::End);
        } else {
            mFileEdit->moveCursor (QTextCursor::Start);
        }
        if(!mFileEdit->find (subString, options)) {
            // restore backup cursor
            mFileEdit->setTextCursor (cursor);
        }
    }
}

void TextEditorWidget::onReplace(const QString &subString, const QString &replaceWith)
{
    auto select = mFileEdit->textCursor ().selectedText ();
    if(select == subString) {
        mFileEdit->insertPlainText (replaceWith);
    }
}

void TextEditorWidget::onReplaceAll(const QString &subString, const QString &replaceWith)
{
    auto document = mFileEdit->document ();
    auto cursor = document->find (subString);
    while(!cursor.isNull ()) {
        cursor.insertText (replaceWith);
        cursor = document->find (subString, cursor);
    }
}

void TextEditorWidget::highlightWord (const QString &subString,QTextDocument::FindFlags options)
{
    QList<QTextEdit::ExtraSelection> extraSelections;
    extraSelections.push_front (mFileEdit->extraSelections ().front ());
    auto document = mFileEdit->document ();
    auto cursor = document->find (subString, 0,
                                  options & (~QTextDocument::FindBackward));
    while(!cursor.isNull ()) {
        QTextEdit::ExtraSelection selection;
        QColor lineColor = QColor(Qt::yellow).lighter(100);
        selection.format.setBackground(lineColor);
        selection.cursor = cursor;
        extraSelections.append(selection);
        cursor = document->find (subString, cursor,
                                 options & (~QTextDocument::FindBackward));
    }
    mFileEdit->setExtraSelections(extraSelections);
}

void TextEditorWidget::reload(int delay) {
    mFileReloadTimer->start(delay);
}

