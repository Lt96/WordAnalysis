//:
// \file
// \author Sean Campbell
// \date   May 2016

#ifndef WORDFREQUENCYFORM_H
#define WORDFREQUENCYFORM_H

#define NUMBER_OF_GRADE_GROUPS 5

#include "wordfrequencymodel.h"
#include <QWidget>
#include <QSortFilterProxyModel>
class QComboBox;
class QTabWidget;
class QTableView;
class QLabel;
class QLineEdit;
class QSpinBox;
class QPushButton;

class WordFrequencyForm : public QWidget
{
        Q_OBJECT
    public:
        explicit WordFrequencyForm(const QString &wdPath = "", QWidget *parent = 0);
        //explicit LibraryForm(rti_word_frequency_list *list, QWidget *parent = 0);
        void setWorkingDirectoryPath(const QString &wdPath) { workingDirectoryPath_ = wdPath; }

    public slots:
        void setWordFrequencyList(rti_word_frequency_list *list);
        void setCompareWordFrequencyList(rti_word_frequency_list *list);

    private slots:
        void updateGeneratedList(int num);
        void browseForFrequencyList();
        void exportFrequencyList();

    private:
        QWidget *createTabWidgetPanel(int index) const;
        void createInterface();
        void layoutInterface();

        // Data
        QString workingDirectoryPath_;
        rti_word_frequency_list *wordFrequencyList_;
        rti_word_frequency_list *compareWordFrequencyList_;
        WordFrequencyModel *wordFrequencyModels[NUMBER_OF_GRADE_GROUPS];
        QSortFilterProxyModel *proxyModels[NUMBER_OF_GRADE_GROUPS];

        // GUI
        QLabel *compareListLabel;
        QLabel *compareListNameLabel;
        QPushButton *browseButton;
        QLabel *searchLabel;
        QLineEdit *searchLineEdit;
        QLabel *numberOfMostFrequentWordsLabel;
        QSpinBox *numberOfMostFrequentWordsSpinBox;
        QPushButton *exportFrequencyListButton;
        QTabWidget *gradeLevelTabWidget;
        QTableView *wordFrequencyViews[NUMBER_OF_GRADE_GROUPS];
        QLabel *gradeLevelFrequencyDescriptionLabels[NUMBER_OF_GRADE_GROUPS];
        QLabel *gradeLevelFrequencyLabels[NUMBER_OF_GRADE_GROUPS*NUMBER_OF_GRADE_GROUPS];
        QLabel *gradeLevelFrequencyValueLabels[NUMBER_OF_GRADE_GROUPS*NUMBER_OF_GRADE_GROUPS];
};

#endif // WORDFREQUENCYFORM_H
