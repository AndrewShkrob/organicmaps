package app.organicmaps.sdk.widget.speed;

import android.view.View;
import androidx.annotation.NonNull;
import app.organicmaps.sdk.util.StringUtils;

public class SpeedViewManager
{
  public interface Config
  {
    float WARNING_THRESHOLD_PERCENTAGE = 0.05f;
    float ALERT_THRESHOLD_PERCENTAGE = 0.10f;
  }

  @NonNull
  private final SpeedView mSpeedView;

  /// Units: meters per second.
  private int mSpeedLimit = -1;
  /// Units: meters per second.
  private int mCurrentSpeed = -1;

  public SpeedViewManager(@NonNull SpeedView speedView)
  {
    mSpeedView = speedView;
  }

  public void show()
  {
    mSpeedView.setVisibility(View.VISIBLE);
  }

  public void hide()
  {
    mSpeedView.setVisibility(View.GONE);
  }

  public void setSpeed(int speedLimit, int currentSpeed)
  {
    if (mSpeedLimit != speedLimit)
    {
      mSpeedLimit = speedLimit;
      mSpeedView.setSpeedLimit(StringUtils.nativeFormatSpeedAndUnits(mSpeedLimit).first);
    }
    if (mCurrentSpeed != currentSpeed)
    {
      mCurrentSpeed = currentSpeed;
      mSpeedView.setCurrentSpeed(StringUtils.nativeFormatSpeedAndUnits(mCurrentSpeed).first);
    }

    updateView();
  }

  private void updateView()
  {
    if (mSpeedLimit <= 0)
    {
      mSpeedView.hideSpeedLimit();
      mSpeedView.setStyle(SpeedView.Style.Normal);
      return;
    }

    mSpeedView.showSpeedLimit();
    if (isSpeedHigherThanAlertThreshold())
      mSpeedView.setStyle(SpeedView.Style.Alert);
    else if (isSpeedHigherThanWarningThreshold())
      mSpeedView.setStyle(SpeedView.Style.Warning);
    else
      mSpeedView.setStyle(SpeedView.Style.Normal);
  }

  private boolean isSpeedHigherThanWarningThreshold()
  {
    return mCurrentSpeed > mSpeedLimit * (1 + Config.WARNING_THRESHOLD_PERCENTAGE);
  }

  private boolean isSpeedHigherThanAlertThreshold()
  {
    return mCurrentSpeed > mSpeedLimit * (1 + Config.ALERT_THRESHOLD_PERCENTAGE);
  }
}
