
#include "tabwidget.h"
#include "mainwindow.h"

TabWidget::TabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    setupModel();
    setupTabs();
}

void TabWidget::setupModel()
{
    model = new MyModel(this);
    model->setTable("climber");
    model->select();
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Nome"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Telefone"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Email"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Vencimento"));
    proxyTextModel = new QSortFilterProxyModel(this);
    proxyTextModel->setSourceModel(model);
}

void TabWidget::setupTabs()
{
    QStringList groups;
    QList<QString> charNames;
    groups << tr("Todos") << tr("Ativos") << tr("Inativos") << tr("Diarios");
    charNames << "" << "A" << "I" << "D";

    for (int i = 0; i < groups.size(); ++i) {
        QString str = groups.at(i);

        proxyModel = new QSortFilterProxyModel(this);
        proxyModel->setSourceModel(proxyTextModel);
        proxyModel->setDynamicSortFilter(true);

        QTableView *tableView = new QTableView;

        tableView->setModel(proxyModel);
        tableView->setSortingEnabled(true);
        tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        //tableView->horizontalHeader()->setStretchLastSection(true);
        tableView->verticalHeader()->hide();
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableView->setSelectionMode(QAbstractItemView::SingleSelection);
        //Date_Start
        tableView->hideColumn(5);
        //Status
        tableView->hideColumn(6);
        //Address
        tableView->hideColumn(2);
        tableView->verticalHeader()->setVisible(false);

        //Connect the select to disable buttons
        QItemSelectionModel *sm = tableView->selectionModel();
        connect(sm, SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
                parentWidget()->parentWidget(), SLOT(rowSelected(QModelIndex,QModelIndex)));

        proxyModel->setFilterRegExp(QRegExp(charNames[i], Qt::CaseInsensitive));
        proxyModel->setFilterKeyColumn(6);
        proxyModel->sort(4, Qt::AscendingOrder);
        addTab(tableView, str);
    }
}

void TabWidget::updateFilter(QString str)
{
    proxyTextModel->setFilterRegExp(str);
}

void TabWidget::insertClimberInDB(Climber *&climber)
{
    qDebug() << climber->getName();
    model->insertClimber(climber);
    QTableView *temp = static_cast<QTableView*>(currentWidget());
    temp->setCurrentIndex(QModelIndex());
}

void TabWidget::removeClimber()
{
    QTableView *temp = static_cast<QTableView*>(currentWidget());
    QItemSelectionModel *selectionModel = temp->selectionModel();
    QModelIndexList indexes = selectionModel->selectedRows();
    QSortFilterProxyModel *proxy = static_cast<QSortFilterProxyModel*>(temp->model());
    QModelIndex idx;
    int row;
    foreach (idx, indexes) {
        row = proxy->mapToSource(idx).row();
        model->removeClimber(row);
        //qDebug() << row << endl;
    }
}