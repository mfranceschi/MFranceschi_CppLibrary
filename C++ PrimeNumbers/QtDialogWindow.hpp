//
// Created by mfran on 16/11/2019.
//

#ifndef TESTET_QTDIALOGWINDOW_HPP
#define TESTET_QTDIALOGWINDOW_HPP

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include "WindowInterface.hpp"

class QtDialogWindow : protected QDialog, public WindowInterface {
    Q_OBJECT
public:
    explicit QtDialogWindow(const ConstructionParams&);
    ~QtDialogWindow() override = default;

    // From WindowInterface
    std::string GetNumber() const override;
    void SetNumberValid(const NumberValidity&) override;
    void SetAnswer(bool) override;
    bool IsCurrentlyInvalid() const override;
    void SetVisible(bool) override;

private:
    QLineEdit* userText = new QLineEdit;
    QLabel* answerLabel = new QLabel("");
    const ConstructionParams* params;

public slots:
    void HandleTextChanged(const QString&);
};


#endif //TESTET_QTDIALOGWINDOW_HPP
