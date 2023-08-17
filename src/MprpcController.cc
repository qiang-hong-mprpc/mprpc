#include "MprpcController.h"

MprpcController::MprpcController()
{
    this->err_text = "";
    this->service_state = false;
}

void MprpcController::Reset()
{
    this->err_text = "";
    this->service_state = false;
}

bool MprpcController::Failed() const
{
    return this->service_state;
}

std::string MprpcController::ErrorText() const
{
    return this->err_text;
}

void MprpcController::SetFailed(const std::string &reason)
{
    // true 代表有问题
    this->service_state = true;
    this->err_text = reason;
}

void MprpcController::StartCancel()
{
}

bool MprpcController::IsCanceled() const
{
    return false;
}

void MprpcController::NotifyOnCancel(google::protobuf::Closure *callback)
{
}