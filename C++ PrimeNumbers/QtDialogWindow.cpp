//
// Created by mfran on 16/11/2019.
//

#include "QtDialogWindow.hpp"
#include <QDebug>
#include <QVBoxLayout>
#include <QLabel>
#include <QtWidgets/QGroupBox>

constexpr static const char* BLACK_STYLESHEET =
        "QLabel { "
        "color: #000000; "
        "}";
constexpr static const char* RED_STYLESHEET =
        "QLabel { "
        "color: #FF0000; "
        "}";

QtDialogWindow::QtDialogWindow(const ConstructionParams & params_in) :
        QDialog(),
        params(&params_in)
{
    auto* gridGroupBox = new QGroupBox();
    auto* layout = new QGridLayout;

    // Set the widgets.
    layout->addWidget(new QLabel(params->label_input_number), 1, 0);
    layout->addWidget(userText, 1, 1);
    layout->addWidget(new QLabel(params->label_before_answer), 2, 0);
    layout->addWidget(answerLabel, 2, 1);
    layout->setColumnStretch(1, 10);
    layout->setColumnStretch(2, 20);
    gridGroupBox->setLayout(layout);

    // Initialize with a number.
    userText->setText(params->textfield_initial_value);

    // Activate signal connection.
    QObject::connect(
            userText, SIGNAL(textChanged(const QString&)),
            this, SLOT(HandleTextChanged(const QString&))
            );

    // Last settings and display.
    QLayout * mainLayout = new QVBoxLayout;
    mainLayout->addWidget(gridGroupBox);
    setLayout(mainLayout);
    setWindowTitle(params->label_window_title);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

std::string QtDialogWindow::GetNumber() const {
    return userText->text().toStdString();
}

void QtDialogWindow::SetNumberValid(const NumberValidity& valid) {
    switch (valid)
    {
        case NumberValidity::VALID:
            answerLabel->setStyleSheet(BLACK_STYLESHEET);
            break;
        case NumberValidity::INCORRECT_INPUT:
            answerLabel->setStyleSheet(RED_STYLESHEET);
            answerLabel->setText(params->output_error_incorrect_input);
            break;
        case NumberValidity::ERROR_TOO_BIG:
            answerLabel->setStyleSheet(RED_STYLESHEET);
            answerLabel->setText(params->output_error_too_big);
            break;
        default:
            answerLabel->setStyleSheet(RED_STYLESHEET);
            answerLabel->setText("U N E X P E C T E D");
            break;
    }
}

void QtDialogWindow::SetAnswer(bool prime) {
    if (!IsCurrentlyInvalid()) {
        if (prime) {
            answerLabel->setText(params->output_if_prime);
        } else {
            answerLabel->setText(params->output_if_not_prime);
        }
    }
}

bool QtDialogWindow::IsCurrentlyInvalid() const {
    return answerLabel->styleSheet() == QLatin1String(RED_STYLESHEET);
}

void QtDialogWindow::HandleTextChanged(const QString &) {
    params->call_on_update();
}

void QtDialogWindow::SetVisible(bool visible) {
    if (visible) {
        show();
    } else {
        hide();
    }
}
