// SPDX-License-Identifier: WTFPL
package aenu.preference;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Color;
import android.util.AttributeSet;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.preference.PreferenceViewHolder;

public class CheckBoxPreference extends androidx.preference.CheckBoxPreference{
    public CheckBoxPreference(@NonNull Context context) {
        this(context, null);
    }
    public CheckBoxPreference(@NonNull Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, androidx.preference.R.attr.checkBoxPreferenceStyle);
    }
    public CheckBoxPreference(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }
    public CheckBoxPreference(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);

        final TypedArray
                a = context.obtainStyledAttributes(attrs,
                new int[]{android.R.attr.textColorPrimary, android.R.attr.textColorSecondary}, defStyleAttr, defStyleRes);
        title_color=a.getColor(0, Color.RED);
        summary_color=a.getColor(1, Color.RED);
        a.recycle();
    }

    int title_color;
    int summary_color;
    boolean is_modify_color=false;
    int modify_color= Color.MAGENTA;

    public int get_modify_color(){
        return modify_color;
    }

    public void set_modify_color(int color){
        modify_color=color;
    }

    public void set_is_modify_color(boolean is_modify_color){
        this.is_modify_color=is_modify_color;
    }

    public boolean get_is_modify_color(){
        return is_modify_color;
    }

    @Override
    public void onBindViewHolder(@NonNull PreferenceViewHolder holder) {
        super.onBindViewHolder(holder);

        TextView title_v=(TextView) holder.itemView.findViewById(android.R.id.title);
        TextView summary_v=(TextView) holder.itemView.findViewById(android.R.id.summary);
        if(is_modify_color){
            if(title_v!=null) title_v.setTextColor(modify_color);
            if(summary_v!=null) summary_v.setTextColor(modify_color);
        }
        else{
            if(title_v!=null) title_v.setTextColor(title_v.isEnabled()?title_color:Color.GRAY);
            if(summary_v!=null) summary_v.setTextColor(summary_v.isEnabled()?summary_color:Color.GRAY);
        }
    }
}
