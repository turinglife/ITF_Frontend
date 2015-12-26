#ifndef ANALYSISMODULE_H
#define ANALYSISMODULE_H

#include <QWidget>
class Delegate;
class QStackedWidget;
class UTaskTree;
class UClipBoard;
class UClipAnalysisBoard;

class AnalysisModule : public QWidget
{
    Q_OBJECT
public:
    explicit AnalysisModule(QWidget *parent = 0);
    ~AnalysisModule();

    void Init();
signals:

private slots:
    void CreateGroup();
    void CreateTask();
    void DeleteGroup();
    void DeleteTask();
    void SelectChannel(QString qstr, int channel);
    void RemoveFromChannel(int channel);
    void ShowClipBoard(int channel);
    void ShowClipAnalysisBoard(int channel);

private:
    void RecoverAnalysisTasks();
    void LoadTaskTree();
    bool LoadGroupsInfo();
    bool LoadTasksInfo();
    void CreateDelegate(std::vector<std::map<std::string, std::string> > &res, int channel);

    QStackedWidget *p_stackedwidget_;
    UTaskTree *p_task_tree_;
    UClipBoard *p_clip_board_;
    UClipAnalysisBoard *p_clip_analysis_board_;
    Delegate *p_delegate_;

    QHash<int, Delegate* > delegate_hash_;
};

#endif // ANALYSISMODULE_H
