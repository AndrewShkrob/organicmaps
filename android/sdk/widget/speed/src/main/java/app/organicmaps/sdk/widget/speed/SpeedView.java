package app.organicmaps.sdk.widget.speed;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Color;
import android.util.AttributeSet;
import android.util.TypedValue;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.TextView;
import androidx.annotation.ColorInt;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.StringRes;
import androidx.constraintlayout.widget.ConstraintLayout;
import app.organicmaps.sdk.widget.speed.internal.BackgroundView;
import app.organicmaps.sdk.widget.speed.internal.SpeedLimitView;

public class SpeedView extends FrameLayout
{
  public enum Style
  {
    Normal,
    Warning,
    Alert
  }

  public enum CurrentSpeedInfoPosition
  {
    Top,
    Bottom,
    Left,
    Right
  }

  @NonNull
  private final BackgroundView mBackgroundView;
  @NonNull
  private final LinearLayout mContainer;
  @NonNull
  private final SpeedLimitView mSpeedLimitView;
  @NonNull
  private final View mCurrentSpeedInfoView;
  @NonNull
  private final TextView mCurrentSpeedTextView;
  @NonNull
  public final TextView mSpeedUnitsTextView;

  private CurrentSpeedInfoPosition mCurrentSpeedInfoPosition = CurrentSpeedInfoPosition.Right;
  private int mSpeedLimitSize;
  @ColorInt
  private int mBackgroundColor;
  @ColorInt
  private int mAlertColor;
  @ColorInt
  private int mTextColor;
  private float mTextSize;

  public SpeedView(@NonNull Context context)
  {
    this(context, null);
  }

  public SpeedView(@NonNull Context context, @Nullable AttributeSet attrs)
  {
    this(context, attrs, 0);
  }

  public SpeedView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr)
  {
    this(context, attrs, defStyleAttr, 0);
  }

  public SpeedView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr, int defStyleRes)
  {
    super(context, attrs, defStyleAttr, defStyleRes);

    inflate(context, R.layout.internal_speed_view, this);
    mBackgroundView = findViewById(R.id._background);
    mContainer = findViewById(R.id._container);
    mSpeedLimitView = findViewById(R.id._speed_limit);
    mCurrentSpeedInfoView = findViewById(R.id._current_speed_info);
    mCurrentSpeedTextView = findViewById(R.id._current_speed);
    mSpeedUnitsTextView = findViewById(R.id._speed_units);

    mSpeedLimitView.setOnTextSizeChangedListener(this::onSpeedLimitViewTextSizeChanged);

    readStyleAttributes(context, attrs, defStyleAttr, defStyleRes);
    update();

    setStyle(Style.Normal);
  }

  public void setSpeedUnits(@StringRes int speedUnitsResId)
  {
    mSpeedUnitsTextView.setText(speedUnitsResId);
  }

  void setStyle(@NonNull Style style)
  {
    switch (style)
    {
    case Normal: setNormalStyle(); break;
    case Warning: setWarningStyle(); break;
    case Alert: setAlertStyle(); break;
    }
    invalidate();
  }

  void setSpeedLimit(@NonNull String speedLimit)
  {
    mSpeedLimitView.setSpeedLimit(speedLimit);
  }

  void setCurrentSpeed(@NonNull String currentSpeed)
  {
    mCurrentSpeedTextView.setText(currentSpeed);
  }

  void hideSpeedLimit()
  {
    if (mSpeedLimitView.getVisibility() == View.GONE)
      return;

    // TODO: add animation
    mSpeedLimitView.setVisibility(View.GONE);

    final ViewGroup.LayoutParams lp = mCurrentSpeedInfoView.getLayoutParams();
    lp.width = mSpeedLimitSize;
    lp.height = mSpeedLimitSize;
    mCurrentSpeedInfoView.setLayoutParams(lp);
  }

  void showSpeedLimit()
  {
    if (mSpeedLimitView.getVisibility() == View.VISIBLE)
      return;

    // TODO: add animation
    mSpeedLimitView.setVisibility(View.VISIBLE);

    final ViewGroup.LayoutParams lp = mCurrentSpeedInfoView.getLayoutParams();
    lp.width = ViewGroup.LayoutParams.WRAP_CONTENT;
    lp.height = ViewGroup.LayoutParams.WRAP_CONTENT;
    mCurrentSpeedInfoView.setLayoutParams(lp);
  }

  @Override
  protected void onSizeChanged(int w, int h, int oldw, int oldh)
  {
    super.onSizeChanged(w, h, oldw, oldh);
    final float cornerRadius = (getHeight() + getPaddingTop() + getPaddingBottom()) / 2f + 100f;
    mBackgroundView.setCornerRadius(cornerRadius);
  }

  private void readStyleAttributes(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr,
                                   int defStyleRes)
  {
    try (TypedArray data =
             context.getTheme().obtainStyledAttributes(attrs, R.styleable.SpeedView, defStyleAttr, defStyleRes))
    {
      mCurrentSpeedInfoPosition = CurrentSpeedInfoPosition.values()[data.getInt(
          R.styleable.SpeedView_current_speed_info_position, CurrentSpeedInfoPosition.Right.ordinal())];
      mSpeedLimitSize = Math.round(data.getDimension(R.styleable.SpeedView_speed_limit_size, 60f));
      mSpeedUnitsTextView.setText(data.getText(R.styleable.SpeedView_speed_units));
      mBackgroundColor = data.getColor(R.styleable.SpeedView_background_color, Color.WHITE);
      mAlertColor = data.getColor(R.styleable.SpeedView_alert_color, Color.RED);
      mTextColor = data.getColor(R.styleable.SpeedView_speed_text_color, Color.BLACK);
      mSpeedLimitView.setBackgroundColor(
          data.getColor(R.styleable.SpeedView_speed_limit_background_color, mBackgroundColor));
      mSpeedLimitView.setBorderColor(data.getColor(R.styleable.SpeedView_speed_limit_border_color, mAlertColor));
      mSpeedLimitView.setTextColor(mTextColor);
    }
  }

  private void update()
  {
    updateOrientation();
    updateSizes();
  }

  private void updateOrientation()
  {
    mContainer.setOrientation((mCurrentSpeedInfoPosition == CurrentSpeedInfoPosition.Top
                               || mCurrentSpeedInfoPosition == CurrentSpeedInfoPosition.Bottom)
                                  ? LinearLayout.VERTICAL
                                  : LinearLayout.HORIZONTAL);

    final int indexOfSpeedLimit = mContainer.indexOfChild(mSpeedLimitView);
    final int indexOfCurrentSpeedInfo = mContainer.indexOfChild(mCurrentSpeedInfoView);

    final boolean currentSpeedInfoShouldBeFirst = (mCurrentSpeedInfoPosition == CurrentSpeedInfoPosition.Top)
                                               || (mCurrentSpeedInfoPosition == CurrentSpeedInfoPosition.Left);

    if (currentSpeedInfoShouldBeFirst && indexOfCurrentSpeedInfo > indexOfSpeedLimit)
    {
      mContainer.removeView(mSpeedLimitView);
      mContainer.removeView(mCurrentSpeedInfoView);

      mContainer.addView(mCurrentSpeedInfoView);
      mContainer.addView(mSpeedLimitView);
    }
    else if (!currentSpeedInfoShouldBeFirst && indexOfSpeedLimit > indexOfCurrentSpeedInfo)
    {
      mContainer.removeView(mSpeedLimitView);
      mContainer.removeView(mCurrentSpeedInfoView);

      mContainer.addView(mSpeedLimitView);
      mContainer.addView(mCurrentSpeedInfoView);
    }
  }

  private void updateSizes()
  {
    mSpeedLimitView.setSize(mSpeedLimitSize);
  }

  private void setNormalStyle()
  {
    mBackgroundView.setColor(mBackgroundColor);
    mCurrentSpeedTextView.setTextColor(mTextColor);
    mSpeedUnitsTextView.setTextColor(mTextColor);
  }

  private void setWarningStyle()
  {
    mBackgroundView.setColor(mBackgroundColor);
    mCurrentSpeedTextView.setTextColor(mAlertColor);
    mSpeedUnitsTextView.setTextColor(mTextColor);
  }

  private void setAlertStyle()
  {
    mBackgroundView.setColor(mAlertColor);
    mCurrentSpeedTextView.setTextColor(mBackgroundColor);
    mSpeedUnitsTextView.setTextColor(mBackgroundColor);
  }

  private void onSpeedLimitViewTextSizeChanged(float textSize)
  {
    mTextSize = textSize;
    mCurrentSpeedTextView.setTextSize(TypedValue.COMPLEX_UNIT_PX, textSize / 1.2f);
    final ConstraintLayout.LayoutParams speedTextLayoutParams =
        (ConstraintLayout.LayoutParams) mCurrentSpeedTextView.getLayoutParams();
    speedTextLayoutParams.width = Math.round(mSpeedLimitView.getMaxTextWidth());
    mCurrentSpeedTextView.setLayoutParams(speedTextLayoutParams);

    mSpeedUnitsTextView.setTextSize(TypedValue.COMPLEX_UNIT_PX, textSize / 2.2f);
    final ConstraintLayout.LayoutParams speedUnitsLayoutParams =
        (ConstraintLayout.LayoutParams) mSpeedUnitsTextView.getLayoutParams();
    speedUnitsLayoutParams.topMargin = (int) -(textSize / 4f);
    mSpeedUnitsTextView.setLayoutParams(speedUnitsLayoutParams);
  }
}
