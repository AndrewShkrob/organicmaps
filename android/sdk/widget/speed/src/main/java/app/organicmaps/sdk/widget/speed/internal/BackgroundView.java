package app.organicmaps.sdk.widget.speed.internal;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;
import androidx.annotation.ColorInt;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.RestrictTo;
import androidx.constraintlayout.widget.ConstraintLayout;

@RestrictTo(RestrictTo.Scope.LIBRARY)
public class BackgroundView extends View
{
  @NonNull
  private final Paint mPaint = new Paint(Paint.ANTI_ALIAS_FLAG);

  @ColorInt
  private int mColor = Color.GRAY;
  private float mCornerRadius = 0f;

  public BackgroundView(@NonNull Context context)
  {
    this(context, null);
  }

  public BackgroundView(@NonNull Context context, @Nullable AttributeSet attrs)
  {
    this(context, attrs, 0);
  }

  public BackgroundView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr)
  {
    this(context, attrs, defStyleAttr, 0);
  }

  public BackgroundView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr, int defStyleRes)
  {
    super(context, attrs, defStyleAttr, defStyleRes);
  }

  public void setColor(@ColorInt int color)
  {
    mColor = color;
    invalidate();
  }

  public void setCornerRadius(float radius)
  {
    mCornerRadius = radius;
    invalidate();
  }

  protected void onDraw(@NonNull Canvas canvas)
  {
    mPaint.setColor(mColor);
    canvas.drawRoundRect(0, 0, getWidth(), getHeight(), mCornerRadius, mCornerRadius, mPaint);
  }

  @Override
  protected void onAttachedToWindow()
  {
    super.onAttachedToWindow();

    final ConstraintLayout.LayoutParams lp = (ConstraintLayout.LayoutParams) getLayoutParams();
    final ViewGroup parent = (ViewGroup) getParent();
    final int padL = parent.getPaddingLeft();
    final int padT = parent.getPaddingTop();
    final int padR = parent.getPaddingRight();
    final int padB = parent.getPaddingBottom();
    lp.setMargins(-padL, -padT, -padR, -padB);
    setLayoutParams(lp);
  }
}
