#include "rti/rti_dictionary.h"
#include "dictionarymodel.h"
#include "dictionaryform.h"
#include <QtWidgets>

//
// Constructors
//
DictionaryForm::DictionaryForm(const QString &wdPath, QWidget *parent) : QWidget(parent)
{
    init(NULL, wdPath);
}

DictionaryForm::DictionaryForm(rti_dictionary *dict, const QString &wdPath, QWidget *parent) : QWidget(parent)
{
    init(dict, wdPath);
}


//
// Public Slots
//
/**
 * @brief DictionaryForm::setDictionary
 * @param dictionary - the new dictionary
 */
void DictionaryForm::setDictionary(rti_dictionary *dictionary)
{
    dictionary_ = dictionary;
    dictionaryModel->setDictionary(dictionary_);
    dictionaryView->resizeColumnsToContents();
    currentDictionaryRadioButton->setChecked(true);
    setWindowModified(true);
}

void DictionaryForm::setMasterDictionary(rti_dictionary *master)
{
    masterDictionary_ = master;
    dictionaryModel->setMasterDictionary(master);
}


//
// Private Slots
//
void DictionaryForm::search(const QString &searchTerm)
{
    QRegExp regExp = QRegExp(searchTerm);
    regExp.setCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterRegExp(regExp);
}

void DictionaryForm::showOnlyIncompleteWords(bool incomplete)
{
    if (incomplete)
    {
        proxyModel->setFilterRegExp("XXX");
        // Ideally, should be MORPHEME_COL and ARPABET_COL, but it would
        // be a good bit more work there isn't time for now, and this will
        // work in most cases by searching every column.
        proxyModel->setFilterKeyColumn(-1);//DictionaryModel::MORPHEME_COL);
    }
    else
    {
        proxyModel->setFilterRegExp(QRegExp());
    }
}

void DictionaryForm::changeDictionary()
{
    if (masterDictionaryRadioButton->isChecked())
        dictionaryModel->setDictionary(masterDictionary_);
    else
        dictionaryModel->setDictionary(dictionary_);
}

void DictionaryForm::importFromMaster()
{
    if (masterDictionary_ == NULL)
    {
        QMessageBox::warning(this, tr("No Master Dictionary"), tr("Cannot import from master dictionary because "
                                                                  "no master dictionary is loaded. Please import "
                                                                  "a master dictionary and try again."));
        return;
    }

    if (dictionary_ == NULL)
    {
        QMessageBox::warning(this, tr("No Dictionary Selected"), tr("Cannot import from master dictionary because "
                                                                    "no dictionary is currently loaded."));
        return;
    }

    bool up_to_date;
    if (dictionary_->import_dictionary(masterDictionary_, &up_to_date))
        QMessageBox::information(this, tr("Success"), tr("Master dictionary imported successfully."));
    masterDictionary_->add_missing_words(dictionary_);
    // If there are any new words in the dictionary, import their information into the master. Since we
    // just wrote all the master's information to the dictionary, we will only change fields of new words
    // in master if there was a difference.
    masterDictionary()->import_dictionary(dictionary_, &up_to_date);
    setWindowModified(true);
}

void DictionaryForm::setModified()
{
    setWindowModified(true);
}

void DictionaryForm::exportDictionary()
{
    QString dictionaryFilePath = QFileDialog::getSaveFileName(this, tr("Choose file to save dictionary into..."),
                                                                    workingDirectoryPath_, "XML files (*.xml)");

    if (!dictionaryFilePath.isEmpty())
    {
        // boolean value isn't used for anything - should double check why it's there
        dictionary_->write_xml(dictionaryFilePath.toStdString(), true);
        setWindowModified(false);
    }
}


//
// Private Methods
//
void DictionaryForm::init(rti_dictionary *dict, const QString &wdPath)
{
    workingDirectoryPath_ = wdPath;
    masterDictionary_ = NULL;
    dictionary_ = dict;
    dictionaryModel = new DictionaryModel(dictionary_);
    connect(dictionaryModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(setModified()));
    proxyModel = new QSortFilterProxyModel;
    proxyModel->setSourceModel(dictionaryModel);

    createInterface();
    layoutInterface();
}

void DictionaryForm::createInterface()
{
    dictionaryView = new QTableView;
    dictionaryView->setModel(proxyModel);
    dictionaryView->setSortingEnabled(true);
    dictionaryView->resizeColumnsToContents();
    dictionaryView->horizontalHeader()->setStretchLastSection(true);

    dictionaryLabel = new QLabel(tr("Dictionary"));
    masterDictionaryRadioButton = new QRadioButton(tr("Master Dictionary"), this);
    currentDictionaryRadioButton = new QRadioButton(tr("Current Dictionary"), this);
    currentDictionaryRadioButton->setChecked(true);
    connect(masterDictionaryRadioButton, SIGNAL(clicked(bool)), this, SLOT(changeDictionary()));
    connect(currentDictionaryRadioButton, SIGNAL(clicked(bool)), this, SLOT(changeDictionary()));
    incompleteWordsCheckBox = new QCheckBox(tr("Show only incomplete words"));
    connect(incompleteWordsCheckBox, SIGNAL(clicked(bool)), this, SLOT(showOnlyIncompleteWords(bool)));
    searchLabel = new QLabel(tr("Search"));
    searchLineEdit = new QLineEdit;
    connect(searchLineEdit, SIGNAL(textChanged(QString)), this, SLOT(search(QString)));
    exportDictionaryButton = new QPushButton(tr("Export Dictionary"));
    connect(exportDictionaryButton, SIGNAL(clicked(bool)), this, SLOT(exportDictionary()));
    importFromMasterButton = new QPushButton(tr("Import From Master"));
    connect(importFromMasterButton, SIGNAL(clicked(bool)), this, SLOT(importFromMaster()));
}

void DictionaryForm::layoutInterface()
{
    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(dictionaryLabel);
    topLayout->addWidget(masterDictionaryRadioButton);
    topLayout->addWidget(currentDictionaryRadioButton);
    topLayout->addStretch();
    topLayout->addWidget(incompleteWordsCheckBox);
    topLayout->addStretch();
    topLayout->addWidget(searchLabel);
    topLayout->addWidget(searchLineEdit);

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(exportDictionaryButton);
    bottomLayout->addStretch();
    bottomLayout->addWidget(importFromMasterButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(dictionaryView);
    mainLayout->addLayout(bottomLayout);
    setLayout(mainLayout);
}
