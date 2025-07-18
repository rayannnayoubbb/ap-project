#include "galleryscreen.h"
#include <QVBoxLayout>

GalleryScreen::GalleryScreen(QWidget *parent) : QWidget(parent)
{

    setFixedSize(800, 450);
    QLabel *background = new QLabel(this);
    background->setPixmap(QPixmap(":/new/prefix1/Picsart_25-04-29_08-39-34-769.png").scaled(800, 450));


    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setGeometry(40, 40, 720, 360);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setStyleSheet("background: transparent; border: none;");

    m_container = new QWidget();
    QGridLayout *grid = new QGridLayout(m_container);
    grid->setSpacing(15);
    grid->setAlignment(Qt::AlignTop);
grid->setContentsMargins(0, 0, 0, 0);


    createAgentCards();
    m_scrollArea->setWidget(m_container);


    QPushButton *backButton = new QPushButton("☰", this);
    backButton->setGeometry(40, 410, 50, 30);
    backButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(89, 168, 73, 0.99);"
        "   border: 1px solid white;"
        "   border-radius: 4px;"
        "   font-size: 18px;"
        "   font-weight: bold;"
        "   color: white;"
        "   padding: 1px;"
        "}"
        "QPushButton:hover { background-color: rgba(255, 221, 0, 0.99); }"
        );
    connect(backButton, &QPushButton::clicked, this, &GalleryScreen::backRequested);
}


void GalleryScreen::createAgentCards()
{
    QGridLayout *grid = qobject_cast<QGridLayout*>(m_container->layout());
    int row = 0, col = 0;

    // ===== 🌊 WATER WALKING AGENTS =====
    addAgentCard(grid, "Billy", "🌊 Water", ":/new/prefix1/agents/Billy.png", 320, 3, 90, 1, row, col++);
    addAgentCard(grid, "Reketon", "🌊 Water", ":/new/prefix1/agents/Reketon.png", 320, 2, 80, 2, row, col++);
    addAgentCard(grid, "Angus", "🌊 Water", ":/new/prefix1/agents/Angus.png", 400, 2, 100, 1, row, col++);
    addAgentCard(grid, "Duraham", "🌊 Water", ":/new/prefix1/agents/Duraham.png", 320, 2, 100, 2, row, col++);

    row++; col = 0;
    addAgentCard(grid, "Colonel Baba", "🌊 Water", ":/new/prefix1/agents/Colonel_Baba.png", 400, 2, 100, 1, row, col++);
    addAgentCard(grid, "Medusa", "🌊 Water", ":/new/prefix1/agents/Medusa.png", 320, 2, 90, 2, row, col++);
    addAgentCard(grid, "Bunka", "🌊 Water", ":/new/prefix1/agents/Bunka.png", 320, 3, 100, 1, row, col++);
    addAgentCard(grid, "Sanka", "🌊 Water", ":/new/prefix1/agents/Sanka.png", 320, 3, 100, 1, row, col++);

    // ===== 🏔️ GROUNDED AGENTS =====
    row++; col = 0;
    addAgentCard(grid, "Sir Lamorak", "🏔️ Grounded", ":/new/prefix1/agents/Sir_Lamorak.png", 320, 3, 110, 1, row, col++);
    addAgentCard(grid, "Kabu", "🏔️ Grounded", ":/new/prefix1/agents/Kabu.png", 400, 2, 120, 1, row, col++);
    addAgentCard(grid, "Rajakal", "🏔️ Grounded", ":/new/prefix1/agents/Rajakal.png", 320, 2, 130, 1, row, col++);
    addAgentCard(grid, "Salih", "🏔️ Grounded", ":/new/prefix1/agents/Salih.png", 400, 2, 80, 1, row, col++);

    row++; col = 0;
    addAgentCard(grid, "Khan", "🏔️ Grounded", ":/new/prefix1/agents/Khan.png", 320, 2, 90, 1, row, col++);
    addAgentCard(grid, "Boi", "🏔️ Grounded", ":/new/prefix1/agents/Boi.png", 400, 2, 100, 1, row, col++);
    addAgentCard(grid, "Eloi", "🏔️ Grounded", ":/new/prefix1/agents/Eloi.png", 240, 2, 100, 2, row, col++);
    addAgentCard(grid, "Kanar", "🏔️ Grounded", ":/new/prefix1/agents/Kanar.png", 160, 2, 100, 2, row, col++);

    row++; col = 0;
    addAgentCard(grid, "Elsa", "🏔️ Grounded", ":/new/prefix1/agents/Elsa.png", 320, 2, 140, 2, row, col++);
    addAgentCard(grid, "Karissa", "🏔️ Grounded", ":/new/prefix1/agents/Karissa.png", 280, 2, 80, 2, row, col++);
    addAgentCard(grid, "Sir Philip", "🏔️ Grounded", ":/new/prefix1/agents/Sir_Philip.png", 400, 2, 100, 1, row, col++);
    addAgentCard(grid, "Frost", "🏔️ Grounded", ":/new/prefix1/agents/Frost.png", 260, 2, 80, 2, row, col++);

    row++; col = 0;
    addAgentCard(grid, "Tusk", "🏔️ Grounded", ":/new/prefix1/agents/Tusk.png", 400, 2, 100, 1, row, col++);

    // ===== 🦅 FLYING AGENTS =====
    addAgentCard(grid, "Rambu", "🦅 Flying", ":/new/prefix1/agents/Rambu.png", 320, 3, 120, 1, row, col++);

    // ===== 👻 FLOATING AGENTS =====
    addAgentCard(grid, "Sabrina", "👻 Floating", ":/new/prefix1/agents/Sabrina.png", 320, 3, 100, 1, row, col++);
    addAgentCard(grid, "Death", "👻 Floating", ":/new/prefix1/agents/Death.png", 240, 3, 120, 2, row, col++);
}






void GalleryScreen::addAgentCard(QGridLayout *grid, const QString &name, const QString &classType,
                                 const QString &imagePath, int hp, int mobility, int damage, int range,
                                 int row, int col)
{
    QWidget *card = new QWidget();
    card->setFixedSize(160, 220);


    QString borderColor;
    if (classType.contains("Water")) borderColor = "2px solid #3498db";
    else if (classType.contains("Grounded")) borderColor = "2px solid #a67c52";
    else if (classType.contains("Flying")) borderColor = "2px solid #ecf0f1";
    else borderColor = "2px solid #9b59b6";

    card->setStyleSheet(QString(
                            "background: rgba(30, 30, 30, 200);"
                            "border-radius: 8px;"
                            "border: %1;"
                            ).arg(borderColor));


    QLabel *imageLabel = new QLabel(card);
    imageLabel->setPixmap(QPixmap(imagePath).scaled(90, 105, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setGeometry(35, 8, 90, 105);
    imageLabel->setStyleSheet("background: rgba(0,0,0,50);");

    // Name and class type
    QLabel *nameLabel = new QLabel(card);
    nameLabel->setText(QString("<center><b>%1</b><br>%2</center>").arg(name).arg(classType));
    nameLabel->setStyleSheet(
        "background-color: black;"
        "color: white ;"
        "font-size: 12px;"
        "border-radius: 4px;"  // Optional: rounded corners
        "padding: 2px;"  // Optional: some inner spacing
        );
    nameLabel->setGeometry(0, 95, 160, 40);  // Below image
    nameLabel->setWordWrap(true);


    QLabel *statsLabel = new QLabel(card);
    statsLabel->setText(QString("❤️ HP: %1\n👟 Move: %2\n⚔️ Dmg: %3\n🎯 Range: %4")
                            .arg(hp).arg(mobility).arg(damage).arg(range));
    statsLabel->setStyleSheet(
        "color: #ccc;"
        "font-size: 11px;"
        "background: rgba(0,0,0,30);"
        "border-radius: 5px;"
        "padding: 3px;");
    statsLabel->setGeometry(10, 140, 140, 70);
    statsLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    grid->addWidget(card, row, col);
}
