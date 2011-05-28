#include <Wt/WContainerWidget>
#include <Wt/WComboBox>
#include <Wt/WDoubleSpinBox>

#include "widgets/TimeDeltaWidget.hpp"
#include "time_intervals.hpp"

namespace thechess {
namespace widgets {

const int intervals_size = 5;
const Td intervals[5] = {second, minute, hour, day, week};

class TimeDeltaWidgetImpl : public Wt::WContainerWidget
{
public:
    TimeDeltaWidgetImpl(const Td& min, const Td& value, const Td& max) :
    Wt::WContainerWidget(), min_(min), max_(max), interval_(second)
    {
        spin_box_ = new Wt::WDoubleSpinBox(this);
        spin_box_->setRange(min.total_seconds(), max.total_seconds());
        spin_box_->setValue(value.total_seconds());
        combo_box_ = new Wt::WComboBox(this);
        combo_box_->addItem(tr("thechess.seconds"));
        combo_box_->addItem(tr("thechess.minutes"));
        combo_box_->addItem(tr("thechess.hours"));
        combo_box_->addItem(tr("thechess.days"));
        combo_box_->addItem(tr("thechess.weeks"));
        combo_box_->activated()
            .connect(this, & TimeDeltaWidgetImpl::select_handler_);
        for (int i = intervals_size - 1; i >= 0; i--)
        {
            if (value > intervals[i])
            {
                combo_box_->setCurrentIndex(i);
                select_handler_(i);
                break;
            }
        }
    }

    Td timedelta() const
    {
        Td td = spin_box_->value() * intervals[combo_box_->currentIndex()];
        if (td > max_)
        {
            td = max_;
        }
        else if (td < min_)
        {
            td = min_;
        }
        return td;
    }

    Wt::WFormWidget* form_widget()
    {
        return spin_box_;
    }

private:
    Wt::WDoubleSpinBox* spin_box_;
    Wt::WComboBox* combo_box_;
    Td min_;
    Td max_;
    Td interval_;

    void select_handler_(int i)
    {
        Td s = spin_box_->value() * interval_;
        interval_ = intervals[i];
        spin_box_->setRange(min_ / interval_, max_ / interval_);
        spin_box_->setValue(s / interval_);
    }
};


TimeDeltaWidget::TimeDeltaWidget(const Td& min, const Td& value,
    const Td& max, Wt::WContainerWidget* parent) :
WCompositeWidget(parent)
{
    impl_ = new TimeDeltaWidgetImpl(min, value, max);
    setImplementation(impl_);
}

Td TimeDeltaWidget::timedelta() const
{
    return impl_->timedelta();
}


Wt::WFormWidget* TimeDeltaWidget::form_widget()
{
    return impl_->form_widget();
}

}
}

