#include <QApplication>
#include <QMainWindow>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QTreeView>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextEdit>
#include <QDialog>
#include <QHeaderView>
#include <QStatusBar>
#include <QSplitter>
#include <QFont>
#include <QDateTime>

class FileManagerWindow final : public QMainWindow {
    Q_OBJECT

public:
    FileManagerWindow() {
        setWindowTitle("File Manager");
        resize(1180, 760);
        setMinimumSize(900, 600);

        auto *central = new QWidget;
        auto *outer = new QHBoxLayout(central);
        outer->setContentsMargins(0, 0, 0, 0);
        outer->setSpacing(0);

        // Sidebar
        auto *sidebar = new QFrame;
        sidebar->setObjectName("sidebar");
        sidebar->setFixedWidth(205);
        auto *side = new QVBoxLayout(sidebar);
        side->setContentsMargins(18, 24, 18, 18);
        side->setSpacing(12);

        auto *brand = new QLabel("▣  FILESPACE");
        brand->setObjectName("brand");
        side->addWidget(brand);
        side->addSpacing(28);

        auto *navTitle = new QLabel("WORKSPACE");
        navTitle->setObjectName("muted");
        side->addWidget(navTitle);

        auto *dashboardBtn = makeButton("▦   Dashboard", "navButton");
        auto *filesBtn = makeButton("▤   All files", "navButton");
        side->addWidget(dashboardBtn);
        side->addWidget(filesBtn);
        side->addStretch();

        auto *rootLabel = new QLabel("WORKING DIRECTORY");
        rootLabel->setObjectName("muted");
        side->addWidget(rootLabel);
        rootPathLabel = new QLabel;
        rootPathLabel->setWordWrap(true);
        rootPathLabel->setObjectName("pathLabel");
        side->addWidget(rootPathLabel);
        auto *chooseRootBtn = makeButton("Choose folder", "secondaryButton");
        side->addWidget(chooseRootBtn);

        // Main content
        auto *content = new QWidget;
        auto *main = new QVBoxLayout(content);
        main->setContentsMargins(28, 24, 28, 24);
        main->setSpacing(18);

        auto *header = new QHBoxLayout;
        auto *headingBox = new QVBoxLayout;
        auto *heading = new QLabel("File manager");
        heading->setObjectName("heading");
        auto *subtitle = new QLabel("Manage files and folders in one place");
        subtitle->setObjectName("muted");
        headingBox->addWidget(heading);
        headingBox->addWidget(subtitle);
        header->addLayout(headingBox);
        header->addStretch();
        searchEdit = new QLineEdit;
        searchEdit->setPlaceholderText("Search in this folder...");
        searchEdit->setClearButtonEnabled(true);
        searchEdit->setFixedWidth(280);
        header->addWidget(searchEdit);
        main->addLayout(header);

        auto *stats = new QGridLayout;
        stats->setSpacing(14);
        filesValue = addStatCard(stats, 0, "FILES", "0");
        foldersValue = addStatCard(stats, 1, "FOLDERS", "0");
        locationValue = addStatCard(stats, 2, "LOCATION", "—");
        main->addLayout(stats);

        auto *toolbar = new QHBoxLayout;
        auto *newFileBtn = makeButton("+ New file", "primaryButton");
        auto *newFolderBtn = makeButton("+ New folder", "secondaryButton");
        auto *renameBtn = makeButton("Rename", "secondaryButton");
        auto *copyBtn = makeButton("Copy", "secondaryButton");
        auto *moveBtn = makeButton("Move", "secondaryButton");
        auto *deleteBtn = makeButton("Delete", "dangerButton");
        auto *openBtn = makeButton("Open / Edit", "secondaryButton");
        toolbar->addWidget(newFileBtn);
        toolbar->addWidget(newFolderBtn);
        toolbar->addSpacing(8);
        toolbar->addWidget(renameBtn);
        toolbar->addWidget(copyBtn);
        toolbar->addWidget(moveBtn);
        toolbar->addWidget(openBtn);
        toolbar->addStretch();
        toolbar->addWidget(deleteBtn);
        main->addLayout(toolbar);

        model = new QFileSystemModel(this);
        model->setReadOnly(false);
        model->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
        proxy = new QSortFilterProxyModel(this);
        proxy->setSourceModel(model);
        proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
        proxy->setFilterKeyColumn(0);
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
        proxy->setRecursiveFilteringEnabled(true);
#endif

        tree = new QTreeView;
        tree->setModel(proxy);
        tree->setAlternatingRowColors(true);
        tree->setSortingEnabled(true);
        tree->sortByColumn(0, Qt::AscendingOrder);
        tree->setSelectionBehavior(QAbstractItemView::SelectRows);
        tree->setSelectionMode(QAbstractItemView::SingleSelection);
        tree->setAnimated(true);
        tree->setIndentation(20);
        tree->header()->setStretchLastSection(false);
        tree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
        tree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        tree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        tree->header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
        main->addWidget(tree, 1);

        auto *footer = new QLabel("Tip: Double-click a folder to browse it. Double-click a file to open it.");
        footer->setObjectName("muted");
        main->addWidget(footer);

        outer->addWidget(sidebar);
        outer->addWidget(content, 1);
        setCentralWidget(central);
        statusBar()->showMessage("Ready");

        connect(chooseRootBtn, &QPushButton::clicked, this, &FileManagerWindow::chooseRoot);
        connect(dashboardBtn, &QPushButton::clicked, this, [this] {
            tree->setRootIndex(proxy->mapFromSource(model->index(rootPath)));
            refreshStats();
        });
        connect(filesBtn, &QPushButton::clicked, this, [this] {
            tree->setRootIndex(proxy->mapFromSource(model->index(rootPath)));
            tree->expandToDepth(0);
        });
        connect(searchEdit, &QLineEdit::textChanged, proxy, &QSortFilterProxyModel::setFilterFixedString);
        connect(newFileBtn, &QPushButton::clicked, this, &FileManagerWindow::createFile);
        connect(newFolderBtn, &QPushButton::clicked, this, &FileManagerWindow::createFolder);
        connect(renameBtn, &QPushButton::clicked, this, &FileManagerWindow::renameSelected);
        connect(copyBtn, &QPushButton::clicked, this, &FileManagerWindow::copySelected);
        connect(moveBtn, &QPushButton::clicked, this, &FileManagerWindow::moveSelected);
        connect(deleteBtn, &QPushButton::clicked, this, &FileManagerWindow::deleteSelected);
        connect(openBtn, &QPushButton::clicked, this, &FileManagerWindow::openSelected);
        connect(tree, &QTreeView::doubleClicked, this, &FileManagerWindow::openIndex);

        setStyleSheet(R"(
            QWidget { background: #f5f7fb; color: #20283a; font-family: "Segoe UI"; font-size: 10pt; }
            #sidebar { background: #17243b; color: #ffffff; }
            #brand { color: #ffffff; font-size: 17pt; font-weight: 700; }
            #muted { color: #8490a5; font-size: 9pt; }
            #sidebar #muted { color: #8290a8; font-weight: 600; }
            #pathLabel { color: #dce4f1; font-size: 9pt; }
            #heading { font-size: 22pt; font-weight: 700; color: #17243b; }
            QLineEdit { background: white; border: 1px solid #dce2ec; border-radius: 8px; padding: 10px 12px; }
            QLineEdit:focus { border: 1px solid #4778dc; }
            QPushButton { border: 0; border-radius: 7px; padding: 9px 13px; font-weight: 600; }
            .primaryButton { background: #3869d4; color: white; }
            .primaryButton:hover { background: #2f5cbd; }
            .secondaryButton { background: #e8edf6; color: #283750; }
            .secondaryButton:hover, .navButton:hover { background: #dce5f4; color: #20314d; }
            .dangerButton { background: #fde9e9; color: #b42332; }
            .dangerButton:hover { background: #fbdada; }
            .navButton { background: transparent; color: #dce4f1; text-align: left; }
            QTreeView { background: white; border: 1px solid #e1e6ef; border-radius: 10px; padding: 8px; alternate-background-color: #f8faff; }
            QTreeView::item { padding: 7px; }
            QTreeView::item:selected { background: #e4edff; color: #1f4fae; }
            QHeaderView::section { background: #f1f4f9; border: 0; padding: 9px; color: #65728a; font-weight: 600; }
            QStatusBar { background: #ffffff; color: #65728a; }
        )");

        setRoot(QDir::homePath());
    }

private:
    QFileSystemModel *model{};
    QSortFilterProxyModel *proxy{};
    QTreeView *tree{};
    QLineEdit *searchEdit{};
    QLabel *rootPathLabel{};
    QLabel *filesValue{};
    QLabel *foldersValue{};
    QLabel *locationValue{};
    QString rootPath;

    QPushButton *makeButton(const QString &text, const QString &className) {
        auto *button = new QPushButton(text);
        button->setProperty("class", className);
        button->setObjectName(className);
        return button;
    }

    QLabel *addStatCard(QGridLayout *grid, int column, const QString &caption, const QString &value) {
        auto *card = new QFrame;
        card->setStyleSheet("QFrame { background: white; border: 1px solid #e5eaf2; border-radius: 10px; }");
        auto *layout = new QVBoxLayout(card);
        layout->setContentsMargins(16, 13, 16, 13);
        auto *title = new QLabel(caption);
        title->setStyleSheet("color:#8290a5; font-size:8pt; font-weight:700; border:0;");
        auto *number = new QLabel(value);
        number->setStyleSheet("color:#1d2b45; font-size:19pt; font-weight:700; border:0;");
        layout->addWidget(title);
        layout->addWidget(number);
        grid->addWidget(card, 0, column);
        return number;
    }

    void setRoot(const QString &path) {
        QDir dir(path);
        if (!dir.exists()) return;
        rootPath = dir.absolutePath();
        const QModelIndex sourceRoot = model->setRootPath(rootPath);
        tree->setRootIndex(proxy->mapFromSource(sourceRoot));
        rootPathLabel->setText(rootPath);
        refreshStats();
        statusBar()->showMessage("Browsing: " + rootPath);
    }

    QString selectedPath() const {
        const QModelIndex proxyIndex = tree->currentIndex();
        if (!proxyIndex.isValid()) return {};
        return model->filePath(proxy->mapToSource(proxyIndex));
    }

    QString selectedDirectory() const {
        const QString path = selectedPath();
        if (path.isEmpty()) return rootPath;
        QFileInfo info(path);
        return info.isDir() ? path : info.absolutePath();
    }

    void chooseRoot() {
        const QString path = QFileDialog::getExistingDirectory(this, "Choose working folder", rootPath);
        if (!path.isEmpty()) setRoot(path);
    }

    void refreshStats() {
        QDir dir(rootPath);
        const QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);
        int fileCount = 0, folderCount = 0;
        for (const QFileInfo &info : entries) info.isDir() ? ++folderCount : ++fileCount;
        filesValue->setText(QString::number(fileCount));
        foldersValue->setText(QString::number(folderCount));
        locationValue->setText(QFileInfo(rootPath).fileName().isEmpty() ? rootPath : QFileInfo(rootPath).fileName());
        rootPathLabel->setText(rootPath);
    }

    void createFile() {
        bool ok = false;
        const QString name = QInputDialog::getText(this, "New file", "File name:", QLineEdit::Normal, {}, &ok);
        if (!ok || name.trimmed().isEmpty()) return;
        const QString path = QDir(selectedDirectory()).filePath(name.trimmed());
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this, "Create file", "Could not create the file:\n" + file.errorString());
            return;
        }
        file.close();
        refreshStats();
        statusBar()->showMessage("File created", 3000);
    }

    void createFolder() {
        bool ok = false;
        const QString name = QInputDialog::getText(this, "New folder", "Folder name:", QLineEdit::Normal, {}, &ok);
        if (!ok || name.trimmed().isEmpty()) return;
        if (!QDir(selectedDirectory()).mkdir(name.trimmed())) {
            QMessageBox::warning(this, "Create folder", "Could not create the folder. Check the name and permissions.");
            return;
        }
        refreshStats();
        statusBar()->showMessage("Folder created", 3000);
    }

    void renameSelected() {
        const QString oldPath = selectedPath();
        if (oldPath.isEmpty()) return;
        const QFileInfo info(oldPath);
        bool ok = false;
        const QString name = QInputDialog::getText(this, "Rename", "New name:", QLineEdit::Normal, info.fileName(), &ok);
        if (!ok || name.trimmed().isEmpty() || name == info.fileName()) return;
        const QString newPath = info.dir().filePath(name.trimmed());
        if (QFileInfo::exists(newPath) || !QFile::rename(oldPath, newPath)) {
            QMessageBox::warning(this, "Rename", "Rename failed. The destination may already exist.");
            return;
        }
        refreshStats();
    }

    void copySelected() {
        const QString source = selectedPath();
        if (source.isEmpty() || QFileInfo(source).isDir()) {
            QMessageBox::information(this, "Copy", "Select a file to copy.");
            return;
        }
        const QString destination = QFileDialog::getSaveFileName(this, "Copy file to", QFileInfo(source).dir().filePath("copy_" + QFileInfo(source).fileName()));
        if (destination.isEmpty()) return;
        if (QFileInfo::exists(destination) || !QFile::copy(source, destination)) {
            QMessageBox::warning(this, "Copy", "Copy failed. The destination may already exist.");
            return;
        }
        refreshStats();
    }

    void moveSelected() {
        const QString source = selectedPath();
        if (source.isEmpty()) return;
        const QFileInfo info(source);
        const QString destination = QFileDialog::getExistingDirectory(
            this, "Choose destination folder", info.absolutePath());
        if (destination.isEmpty()) return;
        const QString target = QDir(destination).filePath(info.fileName());
        if (QFileInfo::exists(target) || !QFile::rename(source, target)) {
            QMessageBox::warning(this, "Move", "Move failed. The destination may already exist.");
            return;
        }
        refreshStats();
    }

    void deleteSelected() {
        const QString path = selectedPath();
        if (path.isEmpty()) return;
        const QFileInfo info(path);
        const auto answer = QMessageBox::question(this, "Delete", "Delete \"" + info.fileName() + "\"?",
                                                   QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (answer != QMessageBox::Yes) return;
        const bool success = info.isDir() ? QDir().rmdir(path) : QFile::remove(path);
        if (!success) {
            QMessageBox::warning(this, "Delete", "Could not delete this item. A folder must be empty.");
            return;
        }
        refreshStats();
    }

    void openSelected() {
        const QString path = selectedPath();
        if (path.isEmpty()) return;
        openIndex(tree->currentIndex());
    }

    void openIndex(const QModelIndex &proxyIndex) {
        if (!proxyIndex.isValid()) return;
        const QString path = model->filePath(proxy->mapToSource(proxyIndex));
        const QFileInfo info(path);
        if (info.isDir()) {
            tree->setRootIndex(proxyIndex);
            statusBar()->showMessage("Browsing: " + path);
            return;
        }

        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Open file", "Could not open the file:\n" + file.errorString());
            return;
        }
        auto *dialog = new QDialog(this);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->setWindowTitle(info.fileName());
        dialog->resize(760, 560);
        auto *layout = new QVBoxLayout(dialog);
        auto *editor = new QTextEdit;
        editor->setPlainText(QString::fromUtf8(file.readAll()));
        file.close();
        auto *save = new QPushButton("Save changes");
        layout->addWidget(editor);
        layout->addWidget(save, 0, Qt::AlignRight);
        connect(save, &QPushButton::clicked, dialog, [this, editor, path, dialog] {
            QFile output(path);
            if (!output.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QMessageBox::warning(dialog, "Save", "Could not save the file:\n" + output.errorString());
                return;
            }
            output.write(editor->toPlainText().toUtf8());
            output.close();
            statusBar()->showMessage("File saved", 3000);
            dialog->accept();
        });
        dialog->exec();
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    FileManagerWindow window;
    window.show();
    return app.exec();
}

#include "main.moc"
