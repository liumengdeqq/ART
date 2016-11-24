//===- MainWindow.h - ART-GUI MainWindow -----------------------*- C++ -*-===//
//
//                     ANDROID REVERSE TOOLKIT
//
// This file is distributed under the GNU GENERAL PUBLIC LICENSE
// V3 License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines ART-GUI MainWindow's behaviour.
//
//===----------------------------------------------------------------------===//
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <BookMark/BookMarkManager.h>
#include <Find/FindDialog.h>
#include <EditorTab/EditorTab.h>
#include <ProjectTab/ProjectTab.h>

#include <QMainWindow>
#include <QCloseEvent>
#include <QTreeView>
#include <QFileSystemModel>
#include <QListWidgetItem>
#include <QTextBrowser>
#include <QVector>


namespace Ui {
class MainWindow;
}
class StatusLabel;
class CommandLineEdit;
class RunDevice;
class MHTabWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

// configuration
    void loadFromConfig();
    void saveToConfig();

    void setupCommandBar();
    void setupStatusBar();

    void setTab(QWidget* widget);
    void loadTabOrder();
protected:
    void dragEnterEvent(QDragEnterEvent* event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent* event) Q_DECL_OVERRIDE;
    void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE;
signals:
    void addCmdMessge(QString msg);
protected slots:
    // File Menu
    void actionExit();
    void actionOpen();
    void actionCloseProject();

    void actionSaveAll();

    void actionOption();

    // Edit Menu
    void actionUndo();
    void actionRedo();
    void actionCut();
    void actionCopy();
    void actionPaste();
    void actionDelete();
    void actionSelectAll();
    void actionFind();
    void actionGotoLine();
    void actionFindAdvance();

    // Help Menu
    void actionAboutArt();

    // Run Menu
    void actionBuild();
    void actionInstall();
    void actionRun();
    void actionDebug();
    void actionStop();
    void actionDevices();

    // cmdLine Message
    void onCmdMessage(QString msg);
    void onCmdClear();

    // fileTree
    void treeFileOpen(const QModelIndex &index);
    void treeFileMenuRequested(const QPoint & point);
    // fileTreeMenuSlot
    void treeFileShowInFile();

    // script Message
    void onProjectOpened(QStringList args);
    void onProjectClosed ();
    void onFindAdvance(QStringList args);

    void onOpenWindow(QStringList args);

    // BookMark message
    void addBookMark(BookMark* pBook, QListWidgetItem *pItem);
    void delBookMark(QListWidgetItem *pItem);
    void bookmarkClick(QListWidgetItem* item);

    // WidgetTab
    void addQWidgetTab(QWidget* qWidget, QString nativeName);
    void addQWidgetTab(QWidget* qWidget);
    void showQWidgetTab(QWidget* qWidget);
    void closeQWidgetTab(QWidget* qWidget);
    void tabMovedSlot(int from, int to);
private:
    void openFile(QString fileName);
    void setProjectDocumentTree(QString path);
    void clearProjectDocumentTree();
private:
    bool needToRebuild ();
private:
    void setupCenterWindows();
    void setupDockWindows();
private:
    Ui::MainWindow *ui;

    StatusLabel* mStatusLabel;
    StatusLabel* mLastLogLabel;
    CommandLineEdit* mCmdLineEdit;

    RunDevice* mRunDevice;

    // DockWindow
    // FileTreeView
    QFileSystemModel *mFileModel = nullptr;
    QTreeView* mFileTreeView = nullptr;
    QDockWidget* mDockFileTree = nullptr;

    // BookMark
    QListWidget* mBookMarkListWidget = nullptr;
    BookMarkManager* mBookMarkManager = nullptr;
    QDockWidget* mDockBookMark = nullptr;

    // Command Area
    QTextBrowser *mCmdTextBrowser = nullptr;
    QDockWidget* mDockCommand = nullptr;
    FindDialog* mFindDialog = nullptr;
    QDockWidget* mDockFind = nullptr;

    // Central Widget
    // Tab Widget
    ProjectTab* mProjectTab;
    EditorTab* mEditorTab;

    MHTabWidget* mTabWidget;
    QVector<QWidget*> mWidgetList;
    QVector<QString> mWidgetNativeNameList;
};

#endif // MAINWINDOW_H
